#include "pch.h"
#include "SSFBXImporter.h"

#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSFBXImporterUtils.h"

#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelCombinationAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IRenderAnimAssetMutable.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"

#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"

#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"

#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"

#include "SSFBXImporter/Public/FRAN.h"

namespace SSFbxName
{
	constexpr char BumpMap[] = "bump_map";
	constexpr char MetalnessMap[] = "metalness_map";
	constexpr char Metalness[] = "metalness";
	constexpr char EmissiveColor[] = "EmissiveColor";
}


SSFBXImporter::SSFBXImporter() :
	_FbxUniqueIDToMtlAsset(1000, 100),
	_IssuedAssetNamesForThisBind(200)
{
	_FBXManager = ::FbxManager::Create();
	_FBXImporter = ::FbxImporter::Create(_FBXManager, "");
}

SSFBXImporter::~SSFBXImporter()
{
	_FBXImporter->Destroy();
	_FBXManager->Destroy();
}

SS::SHasherW SSFBXImporter::GetBoundFilePath() const
{
	return _boundFilePath;
}

SS::SHasherW SSFBXImporter::GetBoundFileName() const
{
	return _boundFileName;
}

SS::SHasherW SSFBXImporter::GetRepresentingAssetName() const
{
	return _RepresentingAssetName;
}

SS::PooledList<IAssetBase*> SSFBXImporter::GetImportedAssets() const
{
	return _ImportedAssets;
}

bool SSFBXImporter::BindFbxSceneFile(const utf16* inFilePath)
{
	_FbxUniqueIDToMtlAsset.Clear();

	uint64 pathLen = wcslen(inFilePath);
	char inFilePathMultiByte[PATH_LEN_MAX];
	int32 writtenBytes = UTF16StrToCharStr(inFilePath, pathLen, inFilePathMultiByte, PATH_LEN_MAX);
	SS_ASSERT(writtenBytes < PATH_LEN_MAX);


	if (!_FBXImporter->Initialize(inFilePathMultiByte)) {
		SS_ASSERT_MSG(false, L"%s", _FBXImporter->GetStatus().GetErrorString());
		return false;
	}

	_currentScene = FbxScene::Create(_FBXManager, inFilePathMultiByte);
	_FBXImporter->Import(_currentScene);

	_boundFilePath = inFilePath;
	SS::StringW FileNameStr;
	ExtractFileNameFromPath(FileNameStr, _boundFilePath.C_Str());
	FileNameStr.Replace(L" ", L"");
	_boundFileName = FileNameStr.C_Str();

	SS::StringW Empty;
	_RepresentingAssetName = IssueNewAssetName(Empty, EAssetType::ModelCombination);

	_importedMeshNames.Clear();
	_importedMeshNames.Reserve(100);


	FbxGlobalSettings& Settings = _currentScene->GetGlobalSettings();
	FbxAxisSystem AxisSystem = Settings.GetAxisSystem();

	int32 FVSign = 0;
	FbxAxisSystem::EFrontVector FV = AxisSystem.GetFrontVector(FVSign);

	int32 UVSign = 0;
	FbxAxisSystem::EUpVector UV = AxisSystem.GetUpVector(UVSign);
	
	FbxAxisSystem::ECoordSystem CS = AxisSystem.GetCoorSystem();

	int32 Up = Settings.GetOriginalUpAxis();
	FbxSystemUnit Unit = Settings.GetOriginalSystemUnit();
	
	// fbx파일을 기본적으로 import하면 나오는 좌표계
	// UpVector (카메라의 윗 방향): Y+
	// FrontVector(카메라가 바라보는 방향): Z+
	// RightVector (카메라의 오른쪽): X-
	// 왼손/오른손: 오른손


	_IssuedAssetNamesForThisBind.Clear();
	
	return true;
}

void SSFBXImporter::ClearFbxSceneFile()
{
	if (_currentScene != nullptr)
	{
		_currentScene->Destroy();
	}
	_currentScene = nullptr;
	_boundFilePath = SS::SHasherW::GetEmpty();
	_boundFileName = SS::SHasherW::GetEmpty();
	_RepresentingAssetName = SS::SHasherW::GetEmpty();
}

void SSFBXImporter::BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager, ICommonRenderAssetSet* InCommonRenderAssetSet)
{
	_AssetManagerToImportAsset = InAssetMnanager;
	_CommonRenderAssetSetToImport = InCommonRenderAssetSet;
}

void SSFBXImporter::ClearRendererToImportAsset()
{
	_AssetManagerToImportAsset = nullptr;
	_CommonRenderAssetSetToImport = nullptr;
}


void SSFBXImporter::RelocateImportedAssetsToAssetManager()
{
	for (IAssetBase* ImportAssetItem : _ImportedAssets)
	{
		_AssetManagerToImportAsset->AddToAssetPool(ImportAssetItem);
	}

	_ImportedAssets.Clear();
}

void SSFBXImporter::GenerateImportedAssets()
{
	GenerateImportedMaterialAssets();
	GenerateImportedMdlcAsset();
	GenerateImportedRenderAnimAssets();
}

IAssetBase* SSFBXImporter::FindImportedAssetByName(SS::SHasherW InAssetName, EAssetType InAssetType) const
{
	for (IAssetBase* ImportedAssetItem : _ImportedAssets)
	{
		if (ImportedAssetItem->GetAssetType() == InAssetType &&
			ImportedAssetItem->GetAssetName() == InAssetName)
		{
			return ImportedAssetItem;
		}
	}

	return nullptr;
}

SS::SHasherW SSFBXImporter::IssueNewAssetName(const SS::StringW& nodeName, EAssetType InAssetType)
{
	SS::StringW NewAssetNameStrPrefix = FRAN::NS_FBX_IMPORT;
	NewAssetNameStrPrefix += L"/";
	NewAssetNameStrPrefix += _boundFileName.C_Str();

	if (nodeName.GetStrLen() > 0)
	{
		NewAssetNameStrPrefix += L"/";
		NewAssetNameStrPrefix += nodeName;
	}


	int32 DupCnt = 0;
	SS::StringW NewAssetNameStrCur;
	while (true)
	{
		NewAssetNameStrCur = NewAssetNameStrPrefix;
		if (DupCnt > 0)
		{
			NewAssetNameStrCur += L"_";
			SS::StringW SuffixNoStr = IntToString(DupCnt++);
			NewAssetNameStrPrefix += SuffixNoStr;
		}

		NewAssetNameStrCur += GetAssetSuffix(InAssetType);

		bool bIsDuplicated = false;
		for (SS::SHasherW IssuedNameItem : _IssuedAssetNamesForThisBind)
		{
			if (wcscmp(NewAssetNameStrCur.C_Str(), IssuedNameItem.C_Str()) == 0)
			{
				DupCnt++;
				bIsDuplicated = true;
				break;
			}
		}

		if (bIsDuplicated == false)
		{
			break;
		}
	}


	SS::SHasherW NewIssuedName = NewAssetNameStrCur.C_Str();
	_IssuedAssetNamesForThisBind.PushBack(NewIssuedName);
	return NewIssuedName;
}

void SSFBXImporter::GenerateImportedMaterialAssets()
{
	static const SS::SHasherW HASHER_FBX_IMPORT = FRAN::NS_FBX_IMPORT;


	const uint32 MtlCnt = _currentScene->GetMaterialCount();

	SS::StringW wsBoundFileName = _boundFileName.C_Str();
	SS::StringW OriginalMtlNodeName;

	SS::StringW TextureAssetPath;
	SS::StringW TextureAssetName;


	constexpr int32 STR_BUFFER_SIZE = 512;
	wchar_t Utf16Buffer[STR_BUFFER_SIZE];

	for (uint32 i = 0; i < MtlCnt; i++)
	{
		const FbxSurfaceMaterial* material = _currentScene->GetMaterial(i);

		FbxString fStrName = material->GetNameOnly();
		int32 StrLen = fStrName.GetLen();
		char8_t* u8Name = reinterpret_cast<char8_t*>(fStrName.Buffer());
		UTF8StrToUTF16Str(reinterpret_cast<char*>(u8Name), StrLen, Utf16Buffer, STR_BUFFER_SIZE);

		OriginalMtlNodeName = Utf16Buffer;
		
		SS::SHasherW MtlAssetName = IssueNewAssetName(OriginalMtlNodeName, EAssetType::Material);
		IMaterialAssetMutable* NewMtlAsset = _AssetManagerToImportAsset->CreateEmptyMaterialAsset(HASHER_FBX_IMPORT, MtlAssetName, _boundFileName);
		MtlDataDefaultPBR* NewDefaultPBRMtlData = DBG_NEW MtlDataDefaultPBR();


		static const SS::SHasherW BlackTexName = CRAN::BLACK_TEX;
		static const SS::SHasherW WhiteTexName = CRAN::WHITE_TEX;
		static const SS::SHasherW EmptyNormalTexName = CRAN::EMPTYNORMAL_TEX;

		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::BaseColor] = WhiteTexName;
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Normal] = EmptyNormalTexName;
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Metallic] = WhiteTexName;
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Emissive] = WhiteTexName;
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Occlusion] = BlackTexName;

		NewDefaultPBRMtlData->_BaseColorScale = {1, 1, 1, 1};
		NewDefaultPBRMtlData->_EmissiveScale = {0, 0, 0, 1};
		NewDefaultPBRMtlData->_NormalTexScale = 1;
		NewDefaultPBRMtlData->_Metallic = 0;
		NewDefaultPBRMtlData->_Roughness = 0;


		NewMtlAsset->InjectRawDataXXX(NewDefaultPBRMtlData);
		_ImportedAssets.PushBack(NewMtlAsset);
		NewMtlAsset->ApplyMtlDataModify();

		uint64 FbxUniqueID = material->GetUniqueID();
		_FbxUniqueIDToMtlAsset.Add(FbxUniqueID, NewMtlAsset);
	}
}

void SSFBXImporter::GenerateImportedMdlcAsset()
{
	static const SS::SHasherW HASHER_FBX_IMPORT = FRAN::NS_FBX_IMPORT;

	if (_currentScene == nullptr) {
		SS_ASSERT_MSG(false, L"No scene to load");
		return;
	}


	FbxNode* rootNode = _currentScene->GetRootNode();
	uint32 childCount = rootNode->GetChildCount();

	const int32 whoeChildCnt = rootNode->GetChildCount(true);
	const int32 rootChildCnt = rootNode->GetChildCount();


	
	IModelCombinationAssetMutable* newMdlcAsset = 
		_AssetManagerToImportAsset->CreateEmptyModelCombinationAsset(HASHER_FBX_IMPORT, _RepresentingAssetName, _boundFilePath.C_Str(), whoeChildCnt);


	for (int32 i = 0; i < rootChildCnt; i++)
	{
		ImportCurrentFileToModelAsset_Recursion(rootNode->GetChild(i), -1, newMdlcAsset);
	}

	MdlcAssetHeader NewHeader;
	NewHeader.ChildCnt = newMdlcAsset->GetChildCnt();
	newMdlcAsset->SetHeader(NewHeader);

	_ImportedAssets.PushBack(newMdlcAsset);

}

void SSFBXImporter::ImportCurrentFileToModelAsset_Recursion(::FbxNode* node, int32 parentReferenceIdx, IModelCombinationAssetMutable* MdlcAsset)
{
	static const SS::SHasherW HASHER_FBX_IMPORT = FRAN::NS_FBX_IMPORT;
	static const SS::SHasherW NAME_EMPTY_PBR_MTL = CRAN::EMPTY_PBR_MTL;


	uint32 childCount = node->GetChildCount();

	constexpr int32 STR_BUFFER_SIZE = 512;
	wchar_t NodeNameBuffer[STR_BUFFER_SIZE];
	FbxString fStrName = node->GetNameOnly();
	int32 StrLen = fStrName.GetLen();
	char8_t* u8Name = reinterpret_cast<char8_t*>(fStrName.Buffer());
	UTF8StrToUTF16Str(reinterpret_cast<char*>(u8Name), StrLen, NodeNameBuffer, STR_BUFFER_SIZE);
	SS::StringW NodeNameString = NodeNameBuffer;

	AssetPlacementReference NewAssetPlacementRef;
	if (node->GetNodeAttribute() != nullptr)
	{

		FbxNodeAttribute::EType nodeAttribute = node->GetNodeAttribute()->GetAttributeType();

		if (nodeAttribute == FbxNodeAttribute::eMesh)
		{
			// geometry asset creation
			::FbxMesh* fbxMesh = node->GetMesh();

			IMeshAsset* newMeshAsset = nullptr;
			SS::SHasherW NewMeshName;

			bool bIsMeshAssetAlreadyImported = false;
			for (const SS::pair<FbxMesh*, SS::SHasherW>& MeshNameItem : _importedMeshNames)
			{
				if (MeshNameItem.first == fbxMesh)
				{
					bIsMeshAssetAlreadyImported = true;
					NewMeshName = MeshNameItem.second;
					break;
				}
			}


			if (bIsMeshAssetAlreadyImported == false)
			{
				
				NewMeshName = IssueNewAssetName(NodeNameString, EAssetType::Mesh);


				if (fbxMesh->GetDeformerCount() == 0)
				{
					newMeshAsset = SSFBXImporterUtils::GenerateNewMeshAssestFromFbxMesh(fbxMesh, NewMeshName, _boundFilePath.C_Str());
				}
				else
				{
					newMeshAsset = SSFBXImporterUtils::GenerateNewSkinnedMeshAssestFromFbxMesh(fbxMesh, NewMeshName, _boundFilePath.C_Str());
				}


				_ImportedAssets.PushBack(newMeshAsset);
				SS::pair<::FbxMesh*, SS::SHasherW> NewPair = SS::MakePair(fbxMesh, NewMeshName);
				_importedMeshNames.PushBack(NewPair);
			}
			else
			{
				newMeshAsset = FindImportedAssetByName<IMeshAsset>(NewMeshName);
			}

			NewAssetPlacementRef.MeshType = newMeshAsset->GetMeshRawData()->GetMeshType();


			
			SS::SHasherW NewModelAssetName = IssueNewAssetName(NodeNameString, EAssetType::Model);

			
			IModelAssetMutable* newModel = _AssetManagerToImportAsset->CreateEmptyModelAsset(HASHER_FBX_IMPORT, NewModelAssetName, _boundFileName);
			SS_ASSERT(newMeshAsset);
			newModel->SetMesh(NewMeshName);


			SS::StringW tempAssetName;
			char outAssetID[10];
			const int32 matCnt = node->GetMaterialCount();
			for (int32 i = 0; i < matCnt; i++) 
			{

				FbxSurfaceMaterial* material = node->GetMaterial(i);
				uint64 MtlFbxUniqueID = material->GetUniqueID();

				IMaterialAsset** ppFoundMtl = _FbxUniqueIDToMtlAsset.Find(MtlFbxUniqueID);
				if (ppFoundMtl == nullptr)
				{
					SS_ASSERT(false);
					newModel->SetMaterial(NAME_EMPTY_PBR_MTL, i);
				}
				else
				{
					IMaterialAsset* FoundMtl = *ppFoundMtl;
					newModel->SetMaterial(FoundMtl->GetAssetName(), i);
				}
			}

			if (matCnt == 0)
			{
				newModel->SetMaterial(NAME_EMPTY_PBR_MTL, 0);
			}


			NewAssetPlacementRef.AssetName = NewModelAssetName;
//			if (tempAssetName.GetStrLen() == 0)
//			{
//				NewAssetPlacementRef.PlacementName = L"Object";
//			}
//			else
//			{
//				NewAssetPlacementRef.PlacementName = tempAssetName.C_Str();
//			}

			_ImportedAssets.PushBack(newModel);
			// PrintFbxNodeInfo(node);
		}
		else if (nodeAttribute == FbxNodeAttribute::eSkeleton)
		{
			NewAssetPlacementRef.MeshType = EMeshType::None;
		}
		else
		{
			NewAssetPlacementRef.MeshType = EMeshType::None;
		}

	}

	NewAssetPlacementRef.PlacementName = NodeNameString.C_Str();
	NewAssetPlacementRef.Transform = SSFBXImporterUtils::ExtractTransformFromNode(node);
	NewAssetPlacementRef.ParentIdx = parentReferenceIdx;
	int32 ThisAssetPlacementIdx = MdlcAsset->GetChildCnt();

	MdlcAsset->AddNewChild(NewAssetPlacementRef);
	for (int32 i = 0; i < childCount; i++)
	{
		ImportCurrentFileToModelAsset_Recursion(node->GetChild(i), ThisAssetPlacementIdx, MdlcAsset);
	}
}

void SSFBXImporter::GenerateImportedRenderAnimAssets()
{
	static const SS::SHasherW HASHER_FBX_IMPORT = FRAN::NS_FBX_IMPORT;

	if (_currentScene == nullptr)
	{
		SS_ASSERT_MSG(false, L"No scene to load");
		return;
	}

	int32 AnimStackCnt = _currentScene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < AnimStackCnt; i++)
	{
		FbxAnimStack* CurAnimStack = _currentScene->GetSrcObject<FbxAnimStack>(i);
		_currentScene->SetCurrentAnimationStack(CurAnimStack);


		constexpr int32 STR_BUFFER_SIZE = 512;
		utf16 u16AnimStackName[STR_BUFFER_SIZE];
		FbxString fCurAnimStackName = CurAnimStack->GetNameOnly();
		int32 StrLen = fCurAnimStackName.GetLen();
		char8_t* u8Name = reinterpret_cast<char8_t*>(fCurAnimStackName.Buffer());
		UTF8StrToUTF16Str(reinterpret_cast<char*>(u8Name), StrLen, u16AnimStackName, STR_BUFFER_SIZE);


		SS::StringW NewRenderAnimNameOnly = u16AnimStackName;

		SS::SHasherW NewRenderAnimName = IssueNewAssetName(NewRenderAnimNameOnly, EAssetType::RenderAnim);


		IModelCombinationAsset* OriginalMdlcAsset = FindImportedAssetByName<IModelCombinationAsset>(_RepresentingAssetName);
		int ChildCnt = OriginalMdlcAsset->GetChildCnt();

		IRenderAnimAssetMutable* NewRenderAnimAsset = _AssetManagerToImportAsset->CreateEmptyRenderAnimAsset(HASHER_FBX_IMPORT, NewRenderAnimName, _boundFileName);

		// ========================================================================================================================
		FbxTakeInfo* takeInfo = _currentScene->GetTakeInfo(fCurAnimStackName);
		FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
		FbxTime end = takeInfo->mLocalTimeSpan.GetStop();


		double frameStartSeconds = start.GetSecondDouble();
		double frameEndSeconds = end.GetSecondDouble();

		int64 frameStart = start.GetFrameCount(FbxTime::eFrames24);
		int64 frameEnd = end.GetFrameCount(FbxTime::eFrames24);
		int64 frameCnt = frameEnd - frameStart;


		RenderAnimRawData* NewRawData = DBG_NEW RenderAnimRawData();
		NewRawData->_Tracks.Reserve(frameCnt);
		NewRawData->_Header.TrackCnt = ChildCnt;
		NewRawData->_Header.KeyFrameDuration = frameEndSeconds - frameStartSeconds;


		NewRenderAnimAsset->SetOriginMdlcAsset(OriginalMdlcAsset);
		NewRenderAnimAsset->InjectRawDataXXX(NewRawData);



		for (int32 ChildIdx = 0; ChildIdx < ChildCnt; ChildIdx++)
		{
			const AssetPlacementReference& ChildItem = OriginalMdlcAsset->GetChildAt(ChildIdx);

			uint32 ChildNodeNameLen = ChildItem.PlacementName.GetStrLen();
			const utf16* ChildNodeName = ChildItem.PlacementName.C_Str();


			RKFTrack& NewTrack = NewRawData->_Tracks.Create();

			if (ChildNodeNameLen > 0)
			{
				char ChildNodeNameStr[PATH_LEN_MAX];

				int32 writtenBytes = UTF16StrToCharStr(ChildNodeName, ChildNodeNameLen, ChildNodeNameStr, PATH_LEN_MAX);
				FbxNode* currentNode = _currentScene->FindNodeByName(ChildNodeNameStr);

				SS_ASSERT(currentNode != nullptr);

				NewTrack._TrackItems.SetSizeDirectly(frameCnt);
				NewTrack._TrackName = ChildItem.PlacementName;


				for (int64 CurFrameIdx = frameStart; CurFrameIdx < frameEnd; ++CurFrameIdx)
				{
					FbxTime currTime;
					currTime.SetFrame(CurFrameIdx, FbxTime::eFrames24);

					int32 FrameIdx = CurFrameIdx - frameStart;
					float CurTimeRatio = (float)FrameIdx / (float)frameCnt;
					NewTrack._TrackItems[FrameIdx]._TimeRatio = CurTimeRatio;
					NewTrack._TrackItems[FrameIdx]._Method = EInterpMethod::Linear;
					NewTrack._TrackItems[FrameIdx]._Padding = 0;
					NewTrack._TrackItems[FrameIdx]._Transform = SSFBXImporterUtils::ExtractTransformFromNode(currentNode, currTime);

					int a = 0;
				}
			}
		}

		_ImportedAssets.PushBack(NewRenderAnimAsset);
	}
}