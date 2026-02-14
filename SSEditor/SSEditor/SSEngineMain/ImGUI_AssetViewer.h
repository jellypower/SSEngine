#pragma once


class IRenderer;
enum class EAssetType : int32;

class ImGUI_AssetViewer
{
public:
	ImGUI_AssetViewer(IRenderer* InRenderer);
	virtual ~ImGUI_AssetViewer();


	void ImGUI_ShowAssetViewer();


	void ImGUI_Show_Asset_RefCnt_Table(EAssetType AssetTypeToShow);
	void ImGUI_AssetManager_Material();
	void ImGUI_AssetManager_Model();


	/// <returns> return selected item name when combo item selected. </returns>
	SS::SHasherW ImGUI_ShowAssetCombo(
		EAssetType InType,
		const utf8* LabelName,
		SS::SHasherW PrevSelectedAssetName, 
		ImGuiComboFlags_ ComboFlags = ImGuiComboFlags_WidthFitPreview);
	

private:
	EAssetType _ImGUI_SelectedAssetManager_Type;
	IRenderer* _Renderer = nullptr;
};
