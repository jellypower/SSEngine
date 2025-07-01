#include "SSFBXImporter.h"

#include "SSEngineDefault/Public/SSContainer/SSString/FixedStringA.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSRenderAssetConverter/Private/SSFBXImporterUtils.h"
#include "SSRenderer/Public/RenderAsset/MeshAssetManager.h"
#include "SSRenderer/Public/RenderAsset/MaterialAssetManager.h"
#include "SSRenderer/Public/RenderAsset/ModelAssetManager.h"
#include "SSRenderer/Public/RenderAsset/ModelCombinationAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelCombinationAsset.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"


SSFBXImporter::SSFBXImporter()
{
	_FBXManager = ::FbxManager::Create();
	_FBXImporter = ::FbxImporter::Create(_FBXManager, "");
}

SSFBXImporter::~SSFBXImporter()
{
	_FBXImporter->Destroy();
	_FBXManager->Destroy();
}

bool SSFBXImporter::BindFbxSceneFile(const utf16* inFilePath)
{
	uint64 pathLen = wcslen(inFilePath);
	char inFilePathMultiByte[PATH_LEN_MAX];
	int32 writtenBytes = UTF16StrToCharStr(inFilePath, pathLen, inFilePathMultiByte, PATH_LEN_MAX);
	SS_ASSERT(writtenBytes < PATH_LEN_MAX);


	if (!_FBXImporter->Initialize(inFilePathMultiByte)) {
		SS_CLASS_WARNING_LOG("%s", _FBXImporter->GetStatus().GetErrorString());
		return false;
	}

	_currentScene = FbxScene::Create(_FBXManager, inFilePathMultiByte);
	_FBXImporter->Import(_currentScene);

	_boundFilePath = inFilePath;
	SS::StringW FileNameStr;
	ExtractFileNameFromPath(FileNameStr, _boundFilePath.C_Str());
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
	_boundFilePath = SS::SHasherW::Empty;
}

void SSFBXImporter::SetRendererToImportAsset(SSRenderer* RendererToImport)
{
	_RendererToImportAsset = RendererToImport;
}

void SSFBXImporter::ImportCurrentFileToAssetManager()
{
	ImportCurrentFileToModelAsset();
}

void SSFBXImporter::ImportCurrentFileToMaterialAsset()
{
	SS::StringW outAssetName;
	const int32 matCnt = _currentScene->GetMaterialCount();

}

void SSFBXImporter::ImportCurrentFileToModelAsset()
{
	if (_currentScene == nullptr) {
		SS_CLASS_WARNING_LOG("No scene to load");
		return;
	}

	FbxNode* rootNode = _currentScene->GetRootNode();
	uint32 childCount = rootNode->GetChildCount();

	SS::StringW assetName;
	assetName = _boundFileName.C_Str();
	assetName += L".mdlc";

	const int32 whoeChildCnt = rootNode->GetChildCount(true);
	const int32 rootChildCnt = rootNode->GetChildCount();
	ModelCombinationAsset* newMdlcAsset = DBG_NEW ModelCombinationAsset(assetName.C_Str(), _boundFilePath.C_Str(), whoeChildCnt + 1);

	AssetPlacementReference RootAssetPlacement;
	RootAssetPlacement.ParentIdx = INVALID_IDX;
	RootAssetPlacement.PlacementName = L"root";
	RootAssetPlacement.Transform = Transform::Identity;
	newMdlcAsset->_childs.PushBack(RootAssetPlacement);


	for (int32 i = 0; i < rootChildCnt; i++)
	{
		ImportCurrentFileToModelAsset_Recursion(rootNode->GetChild(i), MDLC_PLACEMENTREF_ROOT_IDX, newMdlcAsset);
	}

	ModelCombinationAssetManager* ModelCombinationAssetManager = _RendererToImportAsset->GetModelCombinationAssetManager();
	ModelCombinationAssetManager->AddToAssetPool(newMdlcAsset);

	// room.fbx/sketchup.001
	// room.fbx / sketchup.001.mdl
	// room.fbx / book
}

void SSFBXImporter::ImportCurrentFileToModelAsset_Recursion(::FbxNode* node, int32 parentReferenceIdx, ModelCombinationAsset* MdlcAsset)
{
	// TODO: importer Skinning 적용 25/01/31

	MeshAssetManager* MeshAssetManager = _RendererToImportAsset->GetMeshAssetManager();
	ModelAssetManager* ModelAssetManager = _RendererToImportAsset->GetModelAssetManager();

	uint32 childCount = node->GetChildCount();

	AssetPlacementReference NewAssetPlacementRef;
	if (node->GetNodeAttribute() != nullptr) {

		FbxNodeAttribute::EType nodeAttribute = node->GetNodeAttribute()->GetAttributeType();

		if (nodeAttribute == FbxNodeAttribute::eMesh)
		{
			// geometry asset creation
			::FbxMesh* fbxMesh = node->GetMesh();

			MeshAsset* newMeshAsset = nullptr;
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
					MeshAssetManager->GenerateAssetName(_boundFileName.C_Str(), fbxMesh->GetNode()->GetName(), L".mesh");


				//			if (fbxMesh->GetDeformerCount() == 0)
				newMeshAsset = SSFBXImporterUtils::GenerateNewMeshAssestFromFbxMesh(fbxMesh, NewMeshName, _boundFilePath.C_Str());
				NewAssetPlacementRef.MeshType = EMeshType::Rigid;
				//			else
				//				newMeshAsset = GenerateSkinnedGeometryFromFbxMesh(fbxMesh);

				MeshAssetManager->AddToAssetPool(newMeshAsset);
				SS::pair<::FbxMesh*, SS::SHasherW> NewPair = SS::MakePair(fbxMesh, NewMeshName);
				_importedMeshNames.PushBack(NewPair);
			}
			else
			{
				newMeshAsset = (MeshAsset*)MeshAssetManager->FindAssetByName(NewMeshName);
			}





			SS::SHasherW NewModelAssetName = 
				ModelAssetManager->GenerateAssetName(_boundFileName.C_Str(), fbxMesh->GetNode()->GetName(), L".mdlc");
			// TODO: 25/03/04 테스트하기

			ModelAsset* newModel = DBG_NEW ModelAsset(NewModelAssetName, _boundFilePath);
			// Room.fbx 파일 열면 메모리 누수 생기는 문제 확인하기.
			newModel->SetMesh(NewMeshName);


			SS::StringW tempAssetName;
			char outAssetID[10];
			const int32 matCnt = node->GetMaterialCount();
			for (int32 i = 0; i < matCnt; i++) {

				FbxSurfaceMaterial* material = node->GetMaterial(i);
				_i64toa_s(material->GetUniqueID(), outAssetID, 10, 10);
				tempAssetName = material->GetNameOnly().Buffer();
				tempAssetName += "_";
				tempAssetName += outAssetID;


				//SSMaterialAsset* modelMaterial = SSMaterialAssetManager::FindAssetWithName(assetName);
				//if (modelMaterial == nullptr)
				//{
				//	modelMaterial = SSMaterialAssetManager::GetEmptyAsset();
				//}
				//else if (newMeshAsset->GetMeshType() == EMeshType::Skinned)
				//{
				//	modelMaterial->ChangeShader(SSShaderAssetManager::SSDefaultPbrSkinnedShaderName);
				//}

				newModel->SetMaterial(tempAssetName.C_Str(), i);

			}
			//if (matCnt == 0)
			//{
			//	MeshAssetManager->CreateTempMaterialAsset()
			//	newModel->SetMaterial(materiala::GetEmptyAsset(), 0);
			//}

			// model combination asset creation
			NewAssetPlacementRef.AssetName = NewModelAssetName;
			if (tempAssetName.GetStrLen() == 0)
			{
				NewAssetPlacementRef.PlacementName = L"Object";
			}
			else
			{
				NewAssetPlacementRef.PlacementName = tempAssetName.C_Str();
			}


			ModelAssetManager->AddToAssetPool(newModel);
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

	MdlcAsset->_childs.PushBack(NewAssetPlacementRef);
	AssetPlacementReference& ParentAssetPlacement = MdlcAsset->_childs[parentReferenceIdx];
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

