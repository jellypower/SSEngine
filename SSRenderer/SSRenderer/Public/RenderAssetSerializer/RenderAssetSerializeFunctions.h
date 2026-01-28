#pragma once
#include "SSRenderer/ModuleExportKeyword.h"


struct MeshDataSerializerContainer;

SSRENDERER_MODULE_NATIVE bool FillDataFromAsset(MeshDataSerializerContainer& Container);
SSRENDERER_MODULE_NATIVE bool FillAssetFromData(MeshDataSerializerContainer& Container);