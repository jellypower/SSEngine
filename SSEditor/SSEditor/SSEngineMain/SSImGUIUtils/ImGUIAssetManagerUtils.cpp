#include "pch.h"
#include "ImGUIAssetManagerUtils.h"

#include <SSFBXImporter/Public/ISSFBXImporter.h>

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


void SSImGUI::TextName(SS::SHasherW Name)
{
	constexpr int32 BUFFER_SIZE = 512;
	utf8 u8StrWorkTable[BUFFER_SIZE];

	const uint32 NameLen = Name.GetStrLen();
	const utf16* NameCStr = Name.C_Str();

	UTF16StrToUtf8Str(NameCStr, NameLen, u8StrWorkTable, BUFFER_SIZE);
	ImGui::Text(u8StrWorkTable);
}

bool ImGUI_Transform_Edit(Transform& InOutTransform)
{
	float PickedScale[3];
	PickedScale[0] = InOutTransform.Scale.X;
	PickedScale[1] = InOutTransform.Scale.Y;
	PickedScale[2] = InOutTransform.Scale.Z;

	float PickedRotation[3];
	Vector4f EulerRotation = XMEulerFromQuaternion(InOutTransform.Rotation.SimdVec);
	PickedRotation[0] = SS::RadToDegrees(EulerRotation.X);
	PickedRotation[1] = SS::RadToDegrees(EulerRotation.Y);
	PickedRotation[2] = SS::RadToDegrees(EulerRotation.Z);

	float PickedPosition[3];
	PickedPosition[0] = InOutTransform.Position.X;
	PickedPosition[1] = InOutTransform.Position.Y;
	PickedPosition[2] = InOutTransform.Position.Z;

	bool bEverEdit = false;

	if (ImGui::InputFloat3("Scale", PickedScale))
	{
		Vector4f NewScale;
		NewScale.X = PickedScale[0];
		NewScale.Y = PickedScale[1];
		NewScale.Z = PickedScale[2];
		NewScale.W = 1;

		InOutTransform.Scale = NewScale;
		bEverEdit = true;
	}


	ImGui::InputFloat3("Rotation", PickedRotation, "%.0f");
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		Vector4f NewRot;

		NewRot.X = SS::DegToRadians(PickedRotation[0]);
		NewRot.Y = SS::DegToRadians(PickedRotation[1]);
		NewRot.Z = SS::DegToRadians(PickedRotation[2]);
		NewRot.W = 0;


		Quaternion NewQuatRot = Quaternion::FromEulerRotation(NewRot);
		InOutTransform.Rotation = NewQuatRot;
		bEverEdit = true;
	}

	if (ImGui::InputFloat3("Position", PickedPosition))
	{
		Vector4f NewPos;
		NewPos.X = PickedPosition[0];
		NewPos.Y = PickedPosition[1];
		NewPos.Z = PickedPosition[2];
		NewPos.W = 1;

		InOutTransform.Position = NewPos;
		bEverEdit = true;
	}

	return bEverEdit;
}


SS::SHasherW ImGUI_ShowAssetListCombo(
	EAssetType InType, 
	const utf8* LabelName, 
	SS::SHasherW PrevSelectedAssetName,
	ImGuiComboFlags_ ComboFlags)
{
	constexpr int32 BUFFER_SIZE = 512;

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
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

SS::SHasherW ImGUI_NamesCombo(
	const SS::SHasherW* ComboItems,
	int32 ComboItemCnt,
	const utf8* LabelName,
	SS::SHasherW SelectedName,
	ImGuiComboFlags_ ComboFlags)
{
	constexpr int32 BUFFER_SIZE = 512;

	uint32 PrevSelectedNSStrLen = SelectedName.GetStrLen();
	const utf16* PrevSelectedNSCStr = SelectedName.C_Str();

	utf8 u8SelectedNS[BUFFER_SIZE] = "EMPTY";
	UTF16StrToUtf8Str(PrevSelectedNSCStr, PrevSelectedNSStrLen, u8SelectedNS, BUFFER_SIZE);

	if (ImGui::BeginCombo(LabelName, u8SelectedNS, ComboFlags))
	{
		for (int32 i = 0; i < ComboItemCnt; i++)
		{
			SS::SHasherW ThisName = ComboItems[i];
			uint32 ThisNameStrLen = ThisName.GetStrLen();
			const utf16* ThisNameCStr = ThisName.C_Str();

			utf8 u8NSItem[BUFFER_SIZE];
			UTF16StrToUtf8Str(ThisNameCStr, ThisNameStrLen, u8NSItem, BUFFER_SIZE);

			if (ImGui::Selectable(u8NSItem, SelectedName == ThisName))
			{
				SelectedName = ThisName;
			}

			if (SelectedName == ThisName)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	return SelectedName;
}

void ImGUI_PrintFbxImporerInfo(const ISSFBXImporter* Importer)
{
	constexpr int32 BUFFER_SIZE = 512;
	utf8 u8StrWorkTable[BUFFER_SIZE] = "EMPTY";

	SS::SHasherW AssetName = Importer->GetRepresentingAssetName();
	uint32 AssetNameLen = AssetName.GetStrLen();
	const utf16* AssetNameCStr = AssetName.C_Str();

	UTF16StrToUtf8Str(AssetNameCStr, AssetNameLen, u8StrWorkTable, BUFFER_SIZE);

	if (ImGui::CollapsingHeader(u8StrWorkTable))
	{
		SS::SHasherW FilePath = Importer->GetBoundFilePath();
		SS::SHasherW FileName = Importer->GetBoundFileName();

		const uint32 FilePathLen = FilePath.GetStrLen();
		const utf16* FilePathCStr = FilePath.C_Str();
		UTF16StrToUtf8Str(FilePathCStr, FilePathLen, u8StrWorkTable, BUFFER_SIZE);
		ImGui::Text("Asset Path: %s", u8StrWorkTable);

		const uint32 FileNameLen = FileName.GetStrLen();
		const utf16* FileNameCStr = FileName.C_Str();
		UTF16StrToUtf8Str(FileNameCStr, FileNameLen, u8StrWorkTable, BUFFER_SIZE);
		ImGui::Text("Asset Name: %s", u8StrWorkTable);


		if (ImGui::TreeNode("Generated Asset Names"))
		{
			if (ImGui::BeginTable("Names", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
			{
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Name");
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Path");


				const SS::PooledList<IAssetBase*>& AssetList = Importer->GetImportedAssets();

				for (const IAssetBase* AssetItem : AssetList)
				{
					const SS::SHasherW AssetItemName = AssetItem->GetAssetName();
					const SS::SHasherW AssetItemPath = AssetItem->GetAssetPath();

					ImGui::TableNextColumn();
					SSImGUI::TextName(AssetItemName);
					ImGui::TableNextColumn();
					SSImGUI::TextName(AssetItemPath);
				}

				ImGui::EndTable();
			}

			ImGui::TreePop();
		}

	}
}
