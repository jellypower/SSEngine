#pragma once


class IAssetDBLoader;
class IRenderer;
enum class EAssetType : int32;
enum class EExportTabbarActionType : int32;

class ImGUI_AssetManager
{
public:
	ImGUI_AssetManager(IRenderer* InRenderer);
	virtual ~ImGUI_AssetManager();

public:
	void PerFrame();

private:
	void ImGUI_ShowAssetViewer();
	void ImGUI_Show_Asset_RefCnt_Table(EAssetType AssetTypeToShow);
	void ImGUI_AssetManager_Material();
	void ImGUI_AssetManager_Model();
	void ImGUI_AssetManager_FBXExporter();


	void ImGUI_ProcessAssetExport();


private:
	void ImGUI_ExportLoadedFBXAssets(SS::SHasherW AssetNameSpace);


private:
	/// <returns> return selected item name when combo item selected. </returns>
	SS::SHasherW ImGUI_ShowAssetCombo(
		EAssetType InType,
		const utf8* LabelName,
		SS::SHasherW PrevSelectedAssetName, 
		ImGuiComboFlags_ ComboFlags = ImGuiComboFlags_WidthFitPreview);
	

private:
	SS::PooledList<byte> _IEDataPool;

	IAssetDBLoader* _AssetDBLoaderToExport = nullptr;
	IRenderer* _Renderer = nullptr;


	EExportTabbarActionType _TabbarActionType;
	EAssetType _ImGUI_SelectedAssetManager_Type;

	SS::SHasherW _SelectedAssetDBNameSpace;
};
