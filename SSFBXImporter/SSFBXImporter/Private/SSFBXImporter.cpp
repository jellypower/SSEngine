#include "SSFBXImporter.h"

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSFBXImporterUtils.h"

#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelCombinationAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IRenderAnimAssetMutable.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"

#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"

#include "SSRenderer/Public/RenderBase/ICommonRenderAssetSet.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"


namespace SSFbxName
{
	constexpr char BumpMap[] = "bump_map";
	constexpr char MetalnessMap[] = "metalness_map";
	constexpr char Metalness[] = "metalness";
	constexpr char EmissiveColor[] = "EmissiveColor";
}
	


SSFBXImporter::SSFBXImporter() :
	_FbxUniqueIDToMtlAsset(1000, 100)
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

void SSFBXImporter::GenerateImportedMaterialAssets()
{
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
		SS::SHasherW MtlAssetName = _AssetManagerToImportAsset->GenerateAssetName(wsBoundFileName, OriginalMtlNodeName, EAssetType::Material);
		IMaterialAssetMutable* NewMtlAsset = _AssetManagerToImportAsset->CreateEmptyMaterialAsset(MtlAssetName, _boundFileName);
		MtlDataDefaultPBR* NewDefaultPBRMtlData = DBG_NEW MtlDataDefaultPBR();


		// ====================================================== Diffuse ====================================================== 
		FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
		if (prop.IsValid())
		{
			FbxDouble3 diffuseFactor = prop.Get<FbxDouble3>();
			NewDefaultPBRMtlData->_BaseColorScale = Vector4f(diffuseFactor[0], diffuseFactor[1], diffuseFactor[2], 1);
		}
		prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if (prop.IsValid())
		{
			const int texCnt = prop.GetSrcObjectCount();
			if (texCnt != 0)
			{
				const FbxFileTexture* fbxTexture = prop.GetSrcObject<FbxFileTexture>();
				TextureAssetPath = fbxTexture->GetFileName();
				ExtractFileNameFromPath(TextureAssetName, TextureAssetPath.C_Str());
				ITextureAsset* TexAssetToBind = FindImportedAssetByName<ITextureAsset>(TextureAssetName.C_Str());

				if (TexAssetToBind != nullptr)
				{
					NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::BaseColor] = TexAssetToBind;
				}
			}
		}
		if (NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::BaseColor] == nullptr)
		{
			NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::BaseColor] =
				_CommonRenderAssetSetToImport->GetTexWHITE();
		}


		// ====================================================== Normal ====================================================== 
		prop = material->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if (prop.IsValid())
		{
			const int texCnt = prop.GetSrcObjectCount();
			if (texCnt != 0)
			{
				const FbxFileTexture* fbxTexture = prop.GetSrcObject<FbxFileTexture>();
				TextureAssetPath = fbxTexture->GetFileName();
				ExtractFileNameFromPath(TextureAssetName, TextureAssetPath.C_Str());
				ITextureAsset* TexAssetToBind = FindImportedAssetByName<ITextureAsset>(TextureAssetName.C_Str());

				if (TexAssetToBind != nullptr)
				{
					NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Normal] = TexAssetToBind;
				}
			}
		}
		if (NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Normal] == nullptr)
		{
			NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Normal] =
				_CommonRenderAssetSetToImport->GetTexEMPTYNORMAL();
		}


		// ====================================================== Emissive ====================================================== 
		prop = material->FindProperty(FbxSurfaceMaterial::sEmissiveFactor);
		if (prop.IsValid())
		{
			FbxDouble emissive = prop.Get<FbxDouble>();
			NewDefaultPBRMtlData->_EmissiveScale = Vector4f(emissive, emissive, emissive, 1);
		}
		prop = material->FindProperty(FbxSurfaceMaterial::sEmissive);
		if (prop.IsValid())
		{
			const int texCnt = prop.GetSrcObjectCount();
			if (texCnt != 0)
			{
				const FbxFileTexture* fbxTexture = prop.GetSrcObject<FbxFileTexture>();
				TextureAssetPath = fbxTexture->GetFileName();
				ExtractFileNameFromPath(TextureAssetName, TextureAssetPath.C_Str());
				ITextureAsset* TexAssetToBind = FindImportedAssetByName<ITextureAsset>(TextureAssetName.C_Str());

				if (TexAssetToBind != nullptr)
				{
					NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Emissive] = TexAssetToBind;
				}
			}
		}
		if (NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Emissive] == nullptr)
		{
			NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Emissive] =
				_CommonRenderAssetSetToImport->GetTexBLACK();
		}


		// ====================================================== Etc Textures ======================================================
		NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Metallic] =
			_CommonRenderAssetSetToImport->GetTexWHITE();
		NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Occlusion] =
			_CommonRenderAssetSetToImport->GetTexBLACK();


		// ====================================================== Add to pool ======================================================
		NewMtlAsset->InjectRawDataXXX(NewDefaultPBRMtlData);
		_ImportedAssets.PushBack(NewMtlAsset);
		NewMtlAsset->NotifyMtlDataModified();

		uint64 FbxUniqueID = material->GetUniqueID();
		_FbxUniqueIDToMtlAsset.Add(FbxUniqueID, NewMtlAsset);
	}
}

void SSFBXImporter::GenerateImportedMdlcAsset()
{
	if (_currentScene == nullptr) {
		SS_ASSERT_MSG(false, L"No scene to load");
		return;
	}


	FbxNode* rootNode = _currentScene->GetRootNode();
	uint32 childCount = rootNode->GetChildCount();

	SS::StringW assetName;
	assetName = _boundFileName.C_Str();
	assetName += L".mdlc";

	const int32 whoeChildCnt = rootNode->GetChildCount(true);
	const int32 rootChildCnt = rootNode->GetChildCount();


	
	IModelCombinationAssetMutable* newMdlcAsset = 
		_AssetManagerToImportAsset->CreateEmptyModelCombinationAsset(assetName.C_Str(), _boundFilePath.C_Str(), whoeChildCnt + 1);
	AssetPlacementReference RootAssetPlacement;
	RootAssetPlacement.ParentIdx = INVALID_IDX;
	RootAssetPlacement.PlacementName = L"root";
	RootAssetPlacement.Transform = Transform::Identity;
	newMdlcAsset->AddNewChild(RootAssetPlacement);


	for (int32 i = 0; i < rootChildCnt; i++)
	{
		ImportCurrentFileToModelAsset_Recursion(rootNode->GetChild(i), MDLC_PLACEMENTREF_ROOT_IDX, newMdlcAsset);
	}

	_ImportedAssets.PushBack(newMdlcAsset);

}

void SSFBXImporter::ImportCurrentFileToModelAsset_Recursion(::FbxNode* node, int32 parentReferenceIdx, IModelCombinationAssetMutable* MdlcAsset)
{
	// TODO: importer Skinning 적용 25/01/31


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
				NewMeshName =
					_AssetManagerToImportAsset->GenerateAssetName(_boundFileName.C_Str(), NodeNameString, EAssetType::Mesh);


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

			SS::SHasherW NewModelAssetName = 
				_AssetManagerToImportAsset->GenerateAssetName(_boundFileName.C_Str(), NodeNameString, EAssetType::Model);

			
			IModelAssetMutable* newModel = _AssetManagerToImportAsset->CreateEmptyModelAsset(NewModelAssetName, _boundFileName);
			SS_ASSERT(newMeshAsset);
			newModel->SetMesh(newMeshAsset);


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
					newModel->SetMaterial(_CommonRenderAssetSetToImport->GetEmptyPBRMaterial(), i);
				}
				else
				{
					newModel->SetMaterial(*ppFoundMtl, i);
				}
			}

			if (matCnt == 0)
			{
				newModel->SetMaterial(_CommonRenderAssetSetToImport->GetEmptyPBRMaterial(), 0);
			}


			NewAssetPlacementRef.AssetName = NewModelAssetName;
			if (tempAssetName.GetStrLen() == 0)
			{
				NewAssetPlacementRef.PlacementName = L"Object";
			}
			else
			{
				NewAssetPlacementRef.PlacementName = tempAssetName.C_Str();
			}

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
	AssetPlacementReference& ParentAssetPlacement = MdlcAsset->GetChildAtMutable(parentReferenceIdx);
	ParentAssetPlacement.ChildIndices.PushBack(ThisAssetPlacementIdx);
	for (int32 i = 0; i < childCount; i++)
	{
		ImportCurrentFileToModelAsset_Recursion(node->GetChild(i), ThisAssetPlacementIdx, MdlcAsset);
	}
}

void SSFBXImporter::GenerateImportedRenderAnimAssets()
{
	if (_currentScene == nullptr)
	{
		SS_ASSERT_MSG(false, L"No scene to load");
		return;
	}


	FbxAnimStack* currAnimStack = _currentScene->GetCurrentAnimationStack();
	if (currAnimStack == nullptr)
	{
		return;
	}		


	SS::StringW OriginalMdlcAssetNameOnly;
	OriginalMdlcAssetNameOnly = _boundFileName.C_Str();

	SS::StringW OriginalMdlcAssetName = OriginalMdlcAssetNameOnly;
	OriginalMdlcAssetName += L".mdlc";



	constexpr int32 STR_BUFFER_SIZE = 512;
	wchar_t Utf16Buffer[STR_BUFFER_SIZE];
	FbxString fStrName = currAnimStack->GetNameOnly();
	int32 StrLen = fStrName.GetLen();
	char8_t* u8Name = reinterpret_cast<char8_t*>(fStrName.Buffer());
	UTF8StrToUTF16Str(reinterpret_cast<char*>(u8Name), StrLen, Utf16Buffer, STR_BUFFER_SIZE);


	SS::StringW NewRenderAnimNameOnly = OriginalMdlcAssetNameOnly;
	NewRenderAnimNameOnly += L"/";
	NewRenderAnimNameOnly += Utf16Buffer;

	SS::SHasherW NewRenderAnimName =
		_AssetManagerToImportAsset->GenerateAssetName(_boundFileName.C_Str(), NewRenderAnimNameOnly, EAssetType::RenderAnim);


	IModelCombinationAsset* OriginalMdlcAsset = FindImportedAssetByName<IModelCombinationAsset>(OriginalMdlcAssetName.C_Str());
	int ChildCnt = OriginalMdlcAsset->GetChildCnt();

	IRenderAnimAssetMutable* NewRenderAnimAsset = _AssetManagerToImportAsset->CreateEmptyRenderAnimAsset(NewRenderAnimName, _boundFileName);

	// ========================================================================================================================
	FbxTakeInfo* takeInfo = _currentScene->GetTakeInfo(fStrName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();


	double frameStartSeconds = start.GetSecondDouble();
	double frameEndSeconds = end.GetSecondDouble();

	int64 frameStart = start.GetFrameCount(FbxTime::eFrames24);
	int64 frameEnd = end.GetFrameCount(FbxTime::eFrames24);
	int64 frameCnt = frameEnd - frameStart;


	RenderAnimRawData* NewRawData = DBG_NEW RenderAnimRawData();
	NewRawData->_Tracks.Reserve(frameCnt);
	NewRawData->_KeyFrameDuration = frameEndSeconds - frameStartSeconds;


	NewRenderAnimAsset->SetOriginMdlcAsset(OriginalMdlcAsset);
	NewRenderAnimAsset->InjectRawDataXXX(NewRawData);


	static const SS::SHasherW NameRoot = SS::SHasherW("root");

	for (int32 i = 0; i < ChildCnt; i++)
	{
		const AssetPlacementReference& ChildItem = OriginalMdlcAsset->GetChildAt(i);

		uint32 ChildNodeNameLen = ChildItem.PlacementName.GetStrLen();
		const utf16* ChildNodeName = ChildItem.PlacementName.C_Str();

		RKFTrack NewTrack;

		if (ChildNodeNameLen > 0 && NameRoot != ChildItem.PlacementName)
		{
			char ChildNodeNameStr[PATH_LEN_MAX];

			int32 writtenBytes = UTF16StrToCharStr(ChildNodeName, ChildNodeNameLen, ChildNodeNameStr, PATH_LEN_MAX);
			FbxNode* currentNode = _currentScene->FindNodeByName(ChildNodeNameStr);
			
			FbxNode* TempNode = _currentScene->FindNodeByName("direction");
			SS_ASSERT(currentNode != nullptr);

			NewTrack._TrackItemCnt = frameCnt;
			NewTrack._Type = ERKFTrackItemType::BoneTransform;
			NewTrack._TrackName = ChildItem.PlacementName;

			RKFTrackItemTransform* NewTrackItems = (RKFTrackItemTransform*)DBG_MALLOC(sizeof(RKFTrackItemTransform) * frameCnt);
			NewTrack._TrackItems = NewTrackItems;


			for (int64 CurFrameIdx = frameStart; CurFrameIdx < frameEnd; ++CurFrameIdx)
			{
				FbxTime currTime;
				currTime.SetFrame(CurFrameIdx, FbxTime::eFrames24);

				int32 FrameIdx = CurFrameIdx - frameStart;
				float CurTimeRatio = (float)FrameIdx / (float)frameCnt;
				NewTrackItems[FrameIdx]._TimeRatio = CurTimeRatio;
				NewTrackItems[FrameIdx]._Transform = SSFBXImporterUtils::ExtractTransformFromNode(currentNode, currTime);

				int a = 0;
			}

			NewRawData->_Tracks.PushBack(NewTrack);

		}
	}

	_ImportedAssets.PushBack(NewRenderAnimAsset);
}