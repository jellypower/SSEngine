#pragma once

enum class EAssetType : int32;
class ISSFBXImporter;

namespace SSImGUI
{
	void TextName(SS::SHasherW Name);
}


bool ImGUI_Transform_Edit(Transform& InOutTransform);

SS::SHasherW ImGUI_ShowAssetListCombo(
	EAssetType InType,
	const utf8* LabelName,
	SS::SHasherW PrevSelectedAssetName,
	ImGuiComboFlags_ ComboFlags = ImGuiComboFlags_WidthFitPreview);


SS::SHasherW ImGUI_NamesCombo(
	const SS::SHasherW* ComboItems,
	int32 ComboItemCnt,
	const utf8* LabelName,
	SS::SHasherW SelectedName,
	ImGuiComboFlags_ ComboFlags = ImGuiComboFlags_None);

void ImGUI_PrintFbxImporerInfo(
	const ISSFBXImporter* Importer);