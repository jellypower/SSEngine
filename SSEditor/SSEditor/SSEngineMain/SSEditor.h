#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/SSString/FixedStringW.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"


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
	void TEMP_CreateAssets();


private:
	SWorld* _DefaultWorld = nullptr;

private:
	IRenderer* _Renderer = nullptr;


	SGameObject* TEMP_PixelPickedObject = nullptr;

	SGameObject* TEMP_MdlcObj = nullptr;
	

	SCameraComponent* TEMP_Camera = nullptr;
	float TEMP_CamXRot = 0;
	float TEMP_CamYRot = 0;
	float TEMP_Speed = 10.f;



private:
	SS::HashMap<SS::SHasherW, SS::FixedStringW<PATH_LEN_MAX>> _hashMap_TMP;
	SS::StringW _importFileName_TMP;

	ISSFBXImporter* _FbxImporter = nullptr;

};
