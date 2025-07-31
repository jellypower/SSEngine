#include "SSFBXImporter.h"

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSFBXImporterUtils.h"

#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelCombinationAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
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


void SSFBXImporter::ImportCurrentFileToAssetManager()
{
	ImportCurrentFileToMaterialAsset();
	ImportCurrentFileToModelAsset();
}

void SSFBXImporter::ImportCurrentFileToMaterialAsset()
{
	const uint32 MtlCnt = _currentScene->GetMaterialCount();

	SS::StringW wsBoundFileName = _boundFileName.C_Str();
	SS::StringW OriginalMtlNodeName;

	SS::StringW TextureAssetPath;
	SS::StringW TextureAssetName;




	for (uint32 i = 0; i < MtlCnt; i++)
	{
		const FbxSurfaceMaterial* material = _currentScene->GetMaterial(i);

		OriginalMtlNodeName = material->GetNameOnly().Buffer();
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
				ITextureAsset* TexAssetToBind = _AssetManagerToImportAsset->FindAssetByName<ITextureAsset>(TextureAssetName.C_Str());

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
				ITextureAsset* TexAssetToBind = _AssetManagerToImportAsset->FindAssetByName<ITextureAsset>(TextureAssetName.C_Str());

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
				ITextureAsset* TexAssetToBind = _AssetManagerToImportAsset->FindAssetByName<ITextureAsset>(TextureAssetName.C_Str());

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
		_AssetManagerToImportAsset->AddToAssetPool(NewMtlAsset);
		NewMtlAsset->NotifyMtlDataModified();

		uint64 FbxUniqueID = material->GetUniqueID();
		_FbxUniqueIDToMtlAsset.Add(FbxUniqueID, NewMtlAsset);
	}
}

void SSFBXImporter::ImportCurrentFileToModelAsset()
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

	_AssetManagerToImportAsset->AddToAssetPool(newMdlcAsset);

}

void SSFBXImporter::ImportCurrentFileToModelAsset_Recursion(::FbxNode* node, int32 parentReferenceIdx, IModelCombinationAssetMutable* MdlcAsset)
{
	// TODO: importer Skinning Àû¿ë 25/01/31


	uint32 childCount = node->GetChildCount();

	AssetPlacementReference NewAssetPlacementRef;
	if (node->GetNodeAttribute() != nullptr) {

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
					_AssetManagerToImportAsset->GenerateAssetName(_boundFileName.C_Str(), fbxMesh->GetNode()->GetName(), EAssetType::Mesh);


				//			if (fbxMesh->GetDeformerCount() == 0)
				newMeshAsset = SSFBXImporterUtils::GenerateNewMeshAssestFromFbxMesh(fbxMesh, NewMeshName, _boundFilePath.C_Str());
				NewAssetPlacementRef.MeshType = EMeshType::Rigid;
				//			else
				//				newMeshAsset = GenerateSkinnedGeometryFromFbxMesh(fbxMesh);

				_AssetManagerToImportAsset->AddToAssetPool(newMeshAsset);
				SS::pair<::FbxMesh*, SS::SHasherW> NewPair = SS::MakePair(fbxMesh, NewMeshName);
				_importedMeshNames.PushBack(NewPair);
			}
			else
			{
				newMeshAsset = _AssetManagerToImportAsset->FindAssetByName<IMeshAsset>(NewMeshName);
			}


			SS::SHasherW NewModelAssetName = 
				_AssetManagerToImportAsset->GenerateAssetName(_boundFileName.C_Str(), fbxMesh->GetNode()->GetName(), EAssetType::Model);

			
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

			_AssetManagerToImportAsset->AddToAssetPool(newModel);
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

	NewAssetPlacementRef.PlacementName = node->GetName();
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

void SSFBXImporter::PrintFbxNodeInfo(FbxNode* node)
{
	::FbxMesh* fbxMesh = node->GetMesh();
	if (fbxMesh == nullptr)
	{
		return;
	}

	{

		SS_LOG("mesh name: %s\n", node->GetName());
		SS_LOG("\tnode ID: %llu, mesh ID: %llu, uv Cnt: %d, material count: %d\n",
			fbxMesh->GetNode()->GetUniqueID(),
			fbxMesh->GetUniqueID(),
			fbxMesh->GetUVLayerCount(),
			node->GetMaterialCount()
		);

		SS_LOG("\tmateria IDs: ");
		for (uint32 i = 0; i < node->GetMaterialCount(); i++)
		{
			SS_LOG("%llu, ", node->GetMaterial(i)->GetUniqueID());
		}
		SS_LOG("\n");

		if (fbxMesh->GetElementNormal())
		{
			SS_LOG("\tnormal count: %d, ", fbxMesh->GetElementNormal()->GetDirectArray().GetCount());
		}
		if (fbxMesh->GetElementUV())
		{
			SS_LOG("uv count: %d, ", fbxMesh->GetElementUV()->GetDirectArray().GetCount());
		}
		SS_LOG("ctrl count: %d, ", fbxMesh->GetControlPointsCount());
		SS_LOG("\n");

		if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon)
		{
			FbxLayerElementArrayTemplate<int>* materialIndices;
			fbxMesh->GetMaterialIndices(&materialIndices);

			SS_LOG("\t(By Polygon) material indice count: %d, polygon count: %d\n", materialIndices->GetCount(), fbxMesh->GetPolygonCount());
		}
		else
		{
			SS_LOG("\t(All Same)\n");
		}
		SS_LOG("\n\n");
	}
}

