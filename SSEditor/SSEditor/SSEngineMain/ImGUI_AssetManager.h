#pragma once

class IAssetBase;
class ISSFBXImporter;
class IAssetDBLoader;
class IRenderer;
enum class EAssetType : int32;
enum class EExportTabbarActionType : int32;


struct NSEditTimePair
{
	SS::SHasherW NameSpace;
	time_t EditTime;
};

class ImGUI_AssetManager
{
private:
	IRenderer* _Renderer = nullptr;

	EExportTabbarActionType _TabbarActionType;
	EAssetType _ImGUI_SelectedAssetManager_Type;


	SS::PooledList<byte> _IEDataPool;
	SS::SHasherW _FbxLoadNSTarget;
	ISSFBXImporter* _FbxImporterToImport = nullptr;
	IAssetDBLoader* _AssetDBLoaderToExport = nullptr;

	SS::PooledList<NSEditTimePair> _LastEditTimes;

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


private:
	void LoadFbxFile();
	void ImportLoadedFbxFile();

	void SaveNameSpaceRecentlyEdited(SS::SHasherW InNameSpace);


	void ReleaseAllAssetList(SS::PooledList<IAssetBase*>& AssetList);
};
