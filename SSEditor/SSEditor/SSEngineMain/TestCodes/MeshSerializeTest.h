#pragma once
#include "SSEngineDefault/Public/SHasher/SHasherW.h"

class IRenderer;

void MeshSerializeTest(IRenderer* InRenderer, SS::SHasherW MeshToTest);
void MdlcSerializeTest(IRenderer* InRenderer, SS::SHasherW MdlcToTest);
void RenderAnimSerializeTest(IRenderer* InRenderer, SS::SHasherW RenderAnimToTest);