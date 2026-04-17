#pragma once
#include "SObject/Public/SObjHashCode.h"

#include "SSEngineDefault/Public/SSContainer/SSString/FixedStringW.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"


class SSGame;
class SColliderBaseComponent;
class ICollDevice;
class ImGUI_Profiler;
class ImGUI_WorldManager;
class IAssetDBLoader;
class ImGUI_AssetManager;
class SRenderLightDirectionalComponent;
enum class EAssetType;
class SSImGUIInitializer;
class ITextureAssetMutable;
class IRenderer;
class SGameObject;
class SCameraComponent;
class SWorld;
class SSEditor;

extern SSEditor* g_Editor;

class SSEditor
{
public:
	SSEditor(IRenderer* EngineRenderer, ICollDevice* EngineCollDevice);
	~SSEditor();

	void StartupEngine();
	void EnginePerFrame();
	void CleanupEngine();



	void TEMP_ProcessContents();
	void EditorControl();


#pragma region IMGUI
private:
	ImGUI_AssetManager* _ImGUI_AssetViewer = nullptr;
	ImGUI_WorldManager* _ImGUI_WorldManager = nullptr;
	ImGUI_Profiler* _ImGUI_Profiler = nullptr;

	void ProcessImGUI();


#pragma endregion

private:
	SWorld* _DefaultWorld = nullptr;
	SSGame* _Game = nullptr;

private:
	IRenderer* _Renderer = nullptr;
	ICollDevice* _CollDevice = nullptr;


	

	SGameObject* TEMP_MdlcObj = nullptr;
	

	SCameraComponent* _FreeCam = nullptr;
	SRenderLightDirectionalComponent* TEMP_Light = nullptr;
	float TEMP_CamXRot = 0;
	float TEMP_CamYRot = 0;
	float TEMP_Speed = 10.f;

	SColliderBaseComponent* TEMP_Coll1 = nullptr;
	SColliderBaseComponent* TEMP_Coll2 = nullptr;


private:
	SS::HashMap<SS::SHasherW, SS::FixedStringW<PATH_LEN_MAX>> _hashMap_TMP;

	IAssetDBLoader* _AssetDBLoader = nullptr;
};
