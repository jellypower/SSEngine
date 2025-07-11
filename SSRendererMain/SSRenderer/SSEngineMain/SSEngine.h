#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/SSString/FixedStringW.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"


class ISSFBXImporter;
class IRenderer;
class SGameObject;
class SCameraComponent;
class SWorld;
class SSEngine;

extern SSEngine* g_Engine;

class SSEngine
{
public:
	SSEngine(IRenderer* EngineRenderer);
	~SSEngine();

	void StartupEngine();
	void EnginePerFrame();
	void CleanupEngine();


	void InjectImportFilePath_TMP(const utf16* inImportFilePath) { _importFileName_TMP = inImportFilePath; }
	void TEMP_CreateTEMPMaterial();
	void TEMP_ProcessContents();

private:
	SWorld* _DefaultWorld = nullptr;

private:
	IRenderer* _Renderer = nullptr;


	SGameObject* TEMP_PixelPickedObject = nullptr;

	SGameObject* TEMP_MdlcObj = nullptr;
	float TEMP_MdlcYRot = 0;
	float TEMP_MdlcXRot = 0;

	SCameraComponent* TEMP_Camera = nullptr;
	float TEMP_CamXRot = 0;
	float TEMP_CamYRot = 0;
	float TEMP_Speed = 10.f;

private:
	SS::HashMap<SS::SHasherW, SS::FixedStringW<PATH_LEN_MAX>> _hashMap_TMP;
	SS::StringW _importFileName_TMP;

	ISSFBXImporter* _FbxImporter = nullptr;
};
