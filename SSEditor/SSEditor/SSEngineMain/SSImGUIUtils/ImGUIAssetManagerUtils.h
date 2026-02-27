#pragma once

enum class EAssetType : int32;

SS::SHasherW ImGUI_ShowAssetListCombo(
	EAssetType InType,
	const utf8* LabelName,
	SS::SHasherW PrevSelectedAssetName,
	ImGuiComboFlags_ ComboFlags = ImGuiComboFlags_WidthFitPreview);

bool ImGUI_Transform_Edit(Transform& InOutTransform);