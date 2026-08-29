#include "pch.h"
#include "ImGUI_AssetManager.h"



#include "ModuleEntryScriptRunner.h"
#include "EngineUtils/PWin32/OpenFilePathDialogue.h"
#include "SSAssetDBManager/Public/IAssetDBLoader.h"

#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"
#include "SSEngineDefault/Public/SystemUtilities.h"
#include "SSEngineDefault/Public/RawInput/SSInput.h"

#include "SSFBXImporter/Public/ISSFBXImporter.h"
#include "SSFBXImporter/Public/FRAN.h"

#include "SSImGUIUtils/ImGUIAssetManagerUtils.h"

#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"
#include "SSRenderer/Public/RenderAssetSerializer/IApakFileReader.h"
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


enum class EExportTabbarActionType : int32
{
	AssetView,
	AssetExport
};


static const SS::SHasherW NAME_SPACES[] =
{
	CRAN::NS_DEFAULT_ASSET,
	FRAN::NS_FBX_IMPORT,
	L"ContentsAssets",
};
constexpr int32 NAME_SPACES_CNT = sizeof(NAME_SPACES) / sizeof(NAME_SPACES[0]);



ImGUI_AssetManager::ImGUI_AssetManager(IRenderer* InRenderer)
{
	_Renderer = InRenderer;
	_ImGUI_SelectedAssetManager_Type = EAssetType::Texture;
	_TabbarActionType = EExportTabbarActionType::AssetView;

	_AssetDBLoaderToExport = g_fpCreateAssetDBLoader();
	_FbxImporterToImport = g_fpCreateSSFBXImporter();
	_FbxLoadNSTarget = CRAN::NS_DEFAULT_ASSET;

	_IEDataPool.Reserve(200);


	time_t CurTime;
	time(&CurTime);
	_LastEditTimes.Reserve(NAME_SPACES_CNT);
	for (int32 i = 0; i < NAME_SPACES_CNT; i++)
	{
		_LastEditTimes.PushBack(
			{ NAME_SPACES[i], CurTime }
		);
	}
}

ImGUI_AssetManager::~ImGUI_AssetManager()
{
	_FbxImporterToImport->ClearFbxSceneFile();
	_FbxImporterToImport->Release();
	_AssetDBLoaderToExport->Release();
}

void ImGUI_AssetManager::PerFrame()
{
	ImGUI_ShowAssetViewer();
}


void ImGUI_AssetManager::ImGUI_ShowAssetViewer()
{
	ImGui::Begin("Asset Editor");
	{
		ImGui::BeginTabBar("AssetManager_Tabbar");

		if (ImGui::TabItemButton("Texture"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Texture;
			_TabbarActionType = EExportTabbarActionType::AssetView;
		}
		else if (ImGui::TabItemButton("Mesh"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Mesh;
			_TabbarActionType = EExportTabbarActionType::AssetView;
		}
		else if (ImGui::TabItemButton("Material"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Material;
			_TabbarActionType = EExportTabbarActionType::AssetView;
		}
		else if (ImGui::TabItemButton("Model"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Model;
			_TabbarActionType = EExportTabbarActionType::AssetView;
		}
		else if (ImGui::TabItemButton("RenderAnim"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::RenderAnim;
			_TabbarActionType = EExportTabbarActionType::AssetView;
		}
		else if (ImGui::TabItemButton("ModelCombination"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::ModelCombination;
			_TabbarActionType = EExportTabbarActionType::AssetView;
		}
		else if (ImGui::TabItemButton("AssetExport"))
		{
			_TabbarActionType = EExportTabbarActionType::AssetExport;
			_ImGUI_SelectedAssetManager_Type = EAssetType::None;
		}
		ImGui::EndTabBar();


		if (_TabbarActionType == EExportTabbarActionType::AssetView)
		{
			switch (_ImGUI_SelectedAssetManager_Type)
			{
			case EAssetType::Texture: ImGUI_Show_Asset_RefCnt_Table(EAssetType::Texture); break;
			case EAssetType::Mesh: ImGUI_Show_Asset_RefCnt_Table(EAssetType::Mesh); break;
			case EAssetType::RenderAnim: ImGUI_Show_Asset_RefCnt_Table(EAssetType::RenderAnim); break;
			case EAssetType::ModelCombination: ImGUI_Show_Asset_RefCnt_Table(EAssetType::ModelCombination); break;
			case EAssetType::Material: ImGUI_AssetManager_Material(); break;
			case EAssetType::Model: ImGUI_AssetManager_Model(); break;
			default:
				SS_ASSERT(false);
				break;
			}
		}
		else if (_TabbarActionType == EExportTabbarActionType::AssetExport)
		{
			ImGUI_AssetManager_FBXExporter();
		}


	}
	ImGui::End();
}

void ImGUI_AssetManager::ImGUI_Show_Asset_RefCnt_Table(EAssetType AssetTypeToShow)
{
	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& AssetList = AssetManager->GetAssetMap(AssetTypeToShow);

	const char* AssetTypeStr = to_string(AssetTypeToShow);

	if (ImGui::BeginTable(AssetTypeStr, 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
	{
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Name");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Path");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Ref Cnt");

		for (const SS::pair<SS::SHasherW, IAssetBase*>& AssetPairItem : AssetList)
		{
			IAssetBase* AssetItem = AssetPairItem.second;
			ImGui::TableNextColumn();

			uint32 AssetStrLen = 0;
			const utf16* AssetCstr = nullptr;

			{
				constexpr int32 BUFFER_SIZE = 256;
				utf8 Converter[BUFFER_SIZE];
				AssetStrLen = AssetItem->GetAssetName().GetStrLen();
				AssetCstr = AssetItem->GetAssetName().C_Str();
				UTF16StrToUtf8Str(AssetCstr, AssetStrLen, Converter, BUFFER_SIZE);

				ImGui::Text(Converter);
			}

			{
				ImGui::TableNextColumn();

				constexpr int32 BUFFER_SIZE = 256;
				utf8 Converter[BUFFER_SIZE];
				AssetStrLen = AssetItem->GetAssetPath().GetStrLen();
				AssetCstr = AssetItem->GetAssetPath().C_Str();
				UTF16StrToUtf8Str(AssetCstr, AssetStrLen, Converter, BUFFER_SIZE);

				ImGui::Text(Converter);
			}

			{
				ImGui::TableNextColumn();

				constexpr int32 BUFFER_SIZE = 256;
				utf8 StrBuffer[BUFFER_SIZE];

				int32 RefCnt = AssetItem->GetAssetInstanceReferenceCnt();
				_itoa(RefCnt, StrBuffer, 10);
				ImGui::Text(StrBuffer);
			}
		}

		ImGui::EndTable();
	}
}

void ImGUI_AssetManager::ImGUI_AssetManager_Material()
{
	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& TextureList = AssetManager->GetAssetMap(EAssetType::Texture);
	const SS::HashMap<SS::SHasherW, IAssetBase*>& MtlList = AssetManager->GetAssetMap(EAssetType::Material);


	for (const SS::pair<SS::SHasherW, IAssetBase*>& MaterialItemPair : MtlList)
	{
		IMaterialAssetMutable* MtlItem = (IMaterialAssetMutable*)MaterialItemPair.second;
		MtlDataBase* MtlData = MtlItem->GetMutableMtlData();
		ImGui::TableNextColumn();

		SS::SHasherW MtlName = MtlItem->GetAssetName();
		uint32 MtlNameStrLen = MtlName.GetStrLen();
		const utf16* MtlNameCStr = MtlName.C_Str();

		SS::SHasherW MtlPath = MtlItem->GetAssetPath();
		uint32 MtlPathStrLen = MtlPath.GetStrLen();
		const utf16* MtlPathCStr = MtlPath.C_Str();

		constexpr int32 BUFFER_SIZE = 256;
		utf8 u8MtlName[BUFFER_SIZE];
		UTF16StrToUtf8Str(MtlNameCStr, MtlNameStrLen, u8MtlName, BUFFER_SIZE);

		utf8 u8MtlPath[BUFFER_SIZE];
		UTF16StrToUtf8Str(MtlPathCStr, MtlPathStrLen, u8MtlPath, BUFFER_SIZE);

		bool bIsMtlEdited = false;

		if (ImGui::CollapsingHeader(u8MtlName))
		{
			ImGui::PushID(u8MtlName);
			{
				// ============================== Mtl Path ==============================
				ImGui::Text("Material Path: %s", u8MtlPath);
				ImGui::Dummy(ImVec2(1, 10));

				//
				if (MtlData->_Type == EMaterialType::DefaultPBR)
				{
					MtlDataDefaultPBR* PbrMtlData = static_cast<MtlDataDefaultPBR*>(MtlData);

					// ============================== Mtl Factor ==============================
					ImGui::Text("Material Factor");
					{
						float BaseColor[4];
						float EmissiveColor[4];
						float NormalTexScale = PbrMtlData->_NormalTexScale;
						float Metallic = PbrMtlData->_Metallic;
						float Roughness = PbrMtlData->_Roughness;

						const Vector4f& v4BaseColor = PbrMtlData->_BaseColorScale;
						const Vector4f& v4EmissiveColor = PbrMtlData->_EmissiveScale;

						memcpy(BaseColor, &v4BaseColor, sizeof(Vector4f));
						memcpy(EmissiveColor, &v4EmissiveColor, sizeof(Vector4f));


						if (ImGui::ColorEdit4("BaseColorFactor", BaseColor))
						{
							memcpy_s(&(PbrMtlData->_BaseColorScale), sizeof(Vector4f),
								BaseColor, sizeof(Vector4f));

							bIsMtlEdited = true;
						}
						if (ImGui::ColorEdit4("EmissiveColorFactor", EmissiveColor))
						{
							memcpy_s(&(PbrMtlData->_EmissiveScale), sizeof(Vector4f),
								EmissiveColor, sizeof(Vector4f));

							bIsMtlEdited = true;
						}
						if (ImGui::SliderFloat("NormalTexScale", &NormalTexScale, 0.0f, 1.0f))
						{
							PbrMtlData->_NormalTexScale = NormalTexScale;
							bIsMtlEdited = true;
						}
						if (ImGui::SliderFloat("Metallic", &Metallic, 0.0f, 1.0f))
						{
							PbrMtlData->_Metallic = Metallic;
							bIsMtlEdited = true;
						}
						if (ImGui::SliderFloat("Roughness", &Roughness, 0.0f, 1.0f))
						{
							PbrMtlData->_Roughness = Roughness;
							bIsMtlEdited = true;
						}
					}

					// ============================== Mtl Textures ==============================
					if (ImGui::TreeNode("Material Textures"))
					{
						for (int32 i = 0; i < (int32)EDefaultPBRMatTexTypes::Count; i++)
						{
							EDefaultPBRMatTexTypes TexType = (EDefaultPBRMatTexTypes)i;
							const char* TexTypeStr = to_string(TexType);
							SS::SHasherW EquippedTexName = PbrMtlData->_TextureAssetNames[i];


							SS::SHasherW NewlySelectedAsset = ImGUI_ShowAssetListCombo(EAssetType::Texture, TexTypeStr, EquippedTexName);
							if (NewlySelectedAsset.IsEmpty() == false)
							{
								PbrMtlData->_TextureAssetNames[i] = NewlySelectedAsset;
								bIsMtlEdited = true;
							}
						}
						ImGui::TreePop();
					}
				}
				else
				{
					SS_ASSERT(false);
				}
			}
			ImGui::PopID();

			if (bIsMtlEdited)
			{
				MtlItem->ApplyMtlDataModify();
			}
		}
	}
}

void ImGUI_AssetManager::ImGUI_AssetManager_Model()
{
	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& ModelList = AssetManager->GetAssetMap(EAssetType::Model);

	for (const SS::pair<SS::SHasherW, IAssetBase*>& ModelItemPair : ModelList)
	{
		IModelAssetMutable* ModelItem = (IModelAssetMutable*)ModelItemPair.second;
		uint32 ModelNameStrLen = ModelItem->GetAssetName().GetStrLen();
		const utf16* ModelNameCStr = ModelItem->GetAssetName().C_Str();

		uint32 ModelPathStrLen = ModelItem->GetAssetPath().GetStrLen();
		const utf16* ModelPathCStr = ModelItem->GetAssetPath().C_Str();

		constexpr int32 BUFFER_SIZE = 256;
		utf8 u8ModelName[BUFFER_SIZE];
		UTF16StrToUtf8Str(ModelNameCStr, ModelNameStrLen, u8ModelName, BUFFER_SIZE);

		utf8 u8ModelPath[BUFFER_SIZE];
		UTF16StrToUtf8Str(ModelPathCStr, ModelPathStrLen, u8ModelPath, BUFFER_SIZE);

		if (ImGui::CollapsingHeader(u8ModelName))
		{
			ImGui::PushID(u8ModelName);
			{
				// Model Path
				ImGui::Text("Model Path: %s", u8ModelPath);
				ImGui::Dummy(ImVec2(1, 10));

				// Mesh Editing
				ImGui::Text("Mesh");
				{
					IMeshAsset* SelectedMesh = ModelItem->GetMeshAsset();
					SS::SHasherW SelectedMeshName = SelectedMesh->GetAssetName();


					SS::SHasherW NewlySelectedMesh = ImGUI_ShowAssetListCombo(EAssetType::Mesh, "Mesh", SelectedMeshName);
					if (NewlySelectedMesh.IsEmpty() == false)
					{
						ModelItem->SetMesh(NewlySelectedMesh);
					}
				}


				ImGui::Dummy(ImVec2(1, 10));
				ImGui::Text("Material");

				// Material Editing
				for (int MtlIdx = 0; MtlIdx < ModelItem->GetSubMeshCnt(); MtlIdx++)
				{
					IMaterialAsset* SelectedMaterial = ModelItem->GetMaterialAsset(MtlIdx);
					SS::SHasherW SelectedMtlName = SelectedMaterial->GetAssetName();


					char MtlHeader[50] = "Material_";
					_itoa(MtlIdx, MtlHeader + 9, 10);


					SS::SHasherW NewlySelectedMtl = ImGUI_ShowAssetListCombo(EAssetType::Material, MtlHeader, SelectedMtlName);
					if (NewlySelectedMtl.IsEmpty() == false)
					{
						ModelItem->SetMaterial(NewlySelectedMtl, MtlIdx);
					}
				}
			}
			ImGui::PopID();
		}

	}
}

void ImGUI_AssetManager::ImGUI_AssetManager_FBXExporter()
{
	_FbxLoadNSTarget = ImGUI_NamesCombo(
		NAME_SPACES,
		NAME_SPACES_CNT,
		"Fbx Load Target",
		_FbxLoadNSTarget,
		ImGuiComboFlags_WidthFitPreview);


	if (ImGui::Button("Load FBX File"))
	{
		LoadFbxFile();
	}

	if (ImGui::Button("Import FBX Asset"))
	{
		ImportLoadedFbxFile();
	}

	if (ImGui::Button("Save Modified Asset"))
	{
		SaveNameSpaceRecentlyEdited(_FbxLoadNSTarget);
	}

	if (_FbxImporterToImport->GetBoundFileName().IsEmpty() == false)
	{
		ImGUI_PrintFbxImporerInfo(_FbxImporterToImport);
	}
}

void ImGUI_AssetManager::LoadFbxFile()
{
	SCOPE_PROFILE(LoadFbxFile);
	SS::StringW OutString;
	HRESULT hr = OpenSystemPathDialogue(OutString);
	if (FAILED(hr))
	{
		SS_ASSERT(false);
		return;
	}

	_FbxImporterToImport->ClearFbxSceneFile();
	_FbxImporterToImport->BindFbxSceneFile(OutString.C_Str(), _FbxLoadNSTarget);
	_FbxImporterToImport->GenerateImportedAssets();
}

void ImGUI_AssetManager::ImportLoadedFbxFile()
{
	SCOPE_PROFILE(FbxImport);

	SS::PooledList<IAssetBase*> AssetListToImport(1024);
	_FbxImporterToImport->RelocateCreatedAssets(AssetListToImport);

	int32 FbxAssetCnt = AssetListToImport.GetSize();
	if (FbxAssetCnt <= 0)
	{
		return;
	}

	SS::PooledList<IAssetBase*> AssetListToApak(1024);
	for (IAssetBase* AssetItem : AssetListToImport)
	{
		const EAssetType AssetType = AssetItem->GetAssetType();
		if (
			AssetType != EAssetType::Mesh &&
			AssetType != EAssetType::ModelCombination &&
			AssetType != EAssetType::RenderAnim
			)
		{
			continue;
		}

		AssetListToApak.PushBack(AssetItem);
	}


	SS::SHasherW ImportedNameSpace = _FbxImporterToImport->GetBoundNameSpace();

	_IEDataPool.Clear();
	AppendApakDataFromAssetList(
		_IEDataPool,
		AssetListToApak,
		ImportedNameSpace,
		ImportedNameSpace);



	SS::StringW OutString;
	HRESULT hr = OpenSystemPathDialogue(OutString, SPD_CREATEPATH);
	if (FAILED(hr))
	{
		ReleaseAllAssetList(AssetListToImport);
		SS_ASSERT(false);
		return;
	}



	bool bResult = ConvertToWorkingDirPath(OutString);
	if (bResult == false)
	{
		ReleaseAllAssetList(AssetListToImport);
		SS_ASSERT(false);
		return;
	}

	SS::SHasherW SaveAssetWorkingDirPath = OutString.C_Str();
	for (IAssetBase* SerializedAssets : AssetListToApak)
	{
		SerializedAssets->SetAssetPathXXX(SaveAssetWorkingDirPath);
	}

	{
		FILE* hFile = nullptr;
		errno_t no = _wfopen_s(&hFile, OutString.C_Str(), L"wb+");
		if (no != 0)
		{
			SS_ASSERT(false);
			return;
		}

		fwrite(_IEDataPool.GetData(), 1, _IEDataPool.GetSize(), hFile);
		fclose(hFile);
	}



	IAssetManagerMutable* AM = _Renderer->GetMutableAssetManager();
	for (IAssetBase* AssetItem : AssetListToImport)
	{
		AM->AddToAssetPool(AssetItem);
	}

	SaveNameSpaceRecentlyEdited(_FbxImporterToImport->GetBoundNameSpace());
	_FbxImporterToImport->ClearFbxSceneFile();
}

void ImGUI_AssetManager::SaveNameSpaceRecentlyEdited(SS::SHasherW InNameSpace)
{
	time_t LastEditTime = 0;
	for (const NSEditTimePair& Item : _LastEditTimes)
	{
		if (Item.NameSpace == InNameSpace)
		{
			LastEditTime = Item.EditTime;
			break;
		}
	}


	SS::PooledList<IAssetBase*> AssetListToSave(256);
	const IAssetManager* AM = _Renderer->GetAssetManager();

	for (int32 i = 0; i < static_cast<int32>(EAssetType::Count); i++)
	{
		const EAssetType Type = static_cast<EAssetType>(i);
		AM->FindAssetsEditSince(AssetListToSave, InNameSpace, Type, LastEditTime);
	}

	_AssetDBLoaderToExport->StartLoadDB(InNameSpace);
	_AssetDBLoaderToExport->PushAssetsToSaveToDB(AssetListToSave);
	_AssetDBLoaderToExport->CreateInterListFromAssetsToSaveToDB(InNameSpace, InNameSpace);
	_AssetDBLoaderToExport->ClearAssetsToSaveToDB();
	_AssetDBLoaderToExport->SaveInterAssetsToDB();
	_AssetDBLoaderToExport->ClearDB();


	time_t CurTime;
	time(&CurTime);

	if (LastEditTime == 0)
	{
		_LastEditTimes.PushBack({ InNameSpace, CurTime });
	}
	else
	{
		for (NSEditTimePair& Item : _LastEditTimes)
		{
			if (Item.NameSpace == InNameSpace)
			{
				Item.EditTime = CurTime;
			}
		}
	}
}

void ImGUI_AssetManager::ReleaseAllAssetList(SS::PooledList<IAssetBase*>& AssetList)
{
	for (IAssetBase* ImportAssetItem : AssetList)
	{
		const EAssetType AssetType = ImportAssetItem->GetAssetType();

		if (AssetType == EAssetType::Mesh)
		{
			IMeshAsset* MeshAssetItem = static_cast<IMeshAsset*>(ImportAssetItem);
			MeshAssetItem->ReleaseSystemData();
			MeshAssetItem->ReleaseGALData();
		}

		ImportAssetItem->Release();
	}

	AssetList.Clear();
}
