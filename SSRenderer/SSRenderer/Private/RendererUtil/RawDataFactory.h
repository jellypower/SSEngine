#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class MeshRawDataSimpleLine;
class MeshRawDataDefault;

MeshRawDataDefault* CreateCube1mRawData();
MeshRawDataDefault* CreateSphere1mRawData(int32 DetailLevel = 4);


MeshRawDataSimpleLine* CreateHemiSphereOutline1m(int32 DetailLevel = 4);