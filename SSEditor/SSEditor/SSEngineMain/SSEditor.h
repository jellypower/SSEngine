#pragma once
#include "SObject/Public/SObjHashCode.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/SSString/FixedStringW.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"


class SRenderLightDirectionalComponent;
enum class EAssetType;
class SSImGUIInitializer;
class ITextureAssetMutable;
class ISSFBXImporter;
class IRenderer;
class SGameObject;
class SCameraComponent;
class SWorld;
class SSEditor;

extern SSEditor* g_Editor;

class SSEditor
{
public:
	SSEditor(IRenderer* EngineRenderer);
	~SSEditor();

	void StartupEngine();
	void EnginePerFrame();
	void CleanupEngine();


	void InjectImportFilePath_TMP(const utf16* inImportFilePath) { _importFileName_TMP = inImportFilePath; }

	void TEMP_CreateAssets();

	void TEMP_ProcessContents();


#pragma region IMGUI
private:
	EAssetType _ImGUI_SelectedAssetManager_Type;


private:
	void ProcessImGUI();


	void ImGUI_AssetManagerWindow();
	void ImGUI_AssetManagerWindow_Texture();
	void ImGUI_AssetManager_Mesh();
	void ImGUI_AssetManager_Material();
	void ImGUI_AssetManager_Model();
	void ImGUI_AssetManager_RenderAnim();

	void ImGUI_FrameInfo();


	void ImGUI_DrawHierarchy();
	void ImGUI_DrawHierarchy_Recursion(SGameObject* Object);

#pragma endregion

private:
	SWorld* _DefaultWorld = nullptr;

private:
	IRenderer* _Renderer = nullptr;


	SObjHashCode _PickedObject = nullptr;

	int32 _PixelPickingRequestFrameCounter = 0;
	SObjHashCode _HieararchyPickedObject = nullptr;
	

	SGameObject* TEMP_MdlcObj = nullptr;
	

	SCameraComponent* TEMP_Camera = nullptr;
	SRenderLightDirectionalComponent* TEMP_Light = nullptr;
	float TEMP_CamXRot = 0;
	float TEMP_CamYRot = 0;
	float TEMP_Speed = 10.f;



private:
	SS::HashMap<SS::SHasherW, SS::FixedStringW<PATH_LEN_MAX>> _hashMap_TMP;
	SS::StringW _importFileName_TMP;

	ISSFBXImporter* _FbxImporter = nullptr;

};
