#pragma once
#include "SObject/Public/SObjHashCode.h"

#include "SSEngineDefault/Public/SSContainer/SSString/FixedStringW.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"


class ImGUI_Profiler;
class ImGUI_WorldManager;
class IAssetDBLoader;
class ImGUI_AssetManager;
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


	void TEMP_ProcessContents();


#pragma region IMGUI
private:
	ImGUI_AssetManager* _ImGUI_AssetViewer = nullptr;
	ImGUI_WorldManager* _ImGUI_WorldManager = nullptr;
	ImGUI_Profiler* _ImGUI_Profiler = nullptr;

	void ProcessImGUI();


#pragma endregion

private:
	SWorld* _DefaultWorld = nullptr;

private:
	IRenderer* _Renderer = nullptr;


	

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
	IAssetDBLoader* _AssetDBLoader = nullptr;
};
