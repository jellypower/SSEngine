#pragma once
#include "SSRenderer/ModuleExportKeyword.h"


struct MeshAssetSerializerContainer;

SSRENDERER_MODULE_NATIVE void FillDataFromAsset(MeshAssetSerializerContainer& Container);
SSRENDERER_MODULE_NATIVE bool FillAssetFromData(MeshAssetSerializerContainer& Container);