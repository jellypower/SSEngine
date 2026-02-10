#include "pch.h"

#include "ImGUI_AssetViewer.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"

#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"

#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"

#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


ImGUI_AssetViewer::ImGUI_AssetViewer(IRenderer* InRenderer)
{
	_Renderer = InRenderer;
	_ImGUI_SelectedAssetManager_Type = EAssetType::Texture;
}

ImGUI_AssetViewer::~ImGUI_AssetViewer()
{

}


void ImGUI_AssetViewer::ImGUI_ShowAssetViewer()
{
	ImGui::Begin("Asset Editor");
	{
		ImGui::BeginTabBar("AssetManager_Tabbar");

		if (ImGui::TabItemButton("Texture"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Texture;
		}
		else if (ImGui::TabItemButton("Mesh"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Mesh;
		}
		else if (ImGui::TabItemButton("Material"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Material;
		}
		else if (ImGui::TabItemButton("Model"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Model;
		}
		else if (ImGui::TabItemButton("RenderAnim"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::RenderAnim;
		}
		else if (ImGui::TabItemButton("ModelCombination"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::ModelCombination;
		}
		ImGui::EndTabBar();

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
	ImGui::End();
}

void ImGUI_AssetViewer::ImGUI_Show_Asset_RefCnt_Table(EAssetType AssetTypeToShow)
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

void ImGUI_AssetViewer::ImGUI_AssetManager_Material()
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


							SS::SHasherW NewlySelectedAsset = ImGUI_ShowAssetCombo(EAssetType::Texture, TexTypeStr, EquippedTexName);
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

void ImGUI_AssetViewer::ImGUI_AssetManager_Model()
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


					SS::SHasherW NewlySelectedMesh = ImGUI_ShowAssetCombo(EAssetType::Mesh, "Mesh", SelectedMeshName);
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


					SS::SHasherW NewlySelectedMtl = ImGUI_ShowAssetCombo(EAssetType::Material, MtlHeader, SelectedMtlName);
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

SS::SHasherW ImGUI_AssetViewer::ImGUI_ShowAssetCombo(EAssetType InType, const utf8* LabelName, SS::SHasherW PrevSelectedAssetName, ImGuiComboFlags_ Flags)
{
	constexpr int32 BUFFER_SIZE = 512;

	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& AssetList = AssetManager->GetAssetMap(InType);

	uint32 PrevSelectedAssetNameStrLen = PrevSelectedAssetName.GetStrLen();
	const utf16* PrevSelectedAssetNameCStr = PrevSelectedAssetName.C_Str();

	utf8 u8EquippedTexName[BUFFER_SIZE] = "EMPTY";
	UTF16StrToUtf8Str(PrevSelectedAssetNameCStr, PrevSelectedAssetNameStrLen, u8EquippedTexName, BUFFER_SIZE);


	SS::SHasherW NewlySelectedAssetName;

	if (ImGui::BeginCombo(LabelName, u8EquippedTexName, ImGuiComboFlags_WidthFitPreview))
	{
		for (const SS::pair<SS::SHasherW, IAssetBase*>& ItemPair : AssetList)
		{
			IAssetBase* AssetItem = ItemPair.second;
			SS::SHasherW AssetItemName = AssetItem->GetAssetName();
			uint32 AssetItemNameStrLen = AssetItemName.GetStrLen();
			const utf16* AssetItemNameCStr = AssetItemName.C_Str();

			utf8 u8SelectTexItemName[BUFFER_SIZE];
			UTF16StrToUtf8Str(AssetItemNameCStr, AssetItemNameStrLen, u8SelectTexItemName, BUFFER_SIZE);

			bool bIsSelectedAsset = false;
			if (PrevSelectedAssetName == AssetItemName)
			{
				bIsSelectedAsset = true;
			}

			if (ImGui::Selectable(u8SelectTexItemName, bIsSelectedAsset))
			{
				NewlySelectedAssetName = AssetItem->GetAssetName();
			}

			if (bIsSelectedAsset)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	return NewlySelectedAssetName;
}
