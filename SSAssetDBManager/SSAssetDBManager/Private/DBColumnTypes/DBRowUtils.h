#pragma once
#include "Rows_v_0.h"

class ITextureAsset;
class IMeshAsset;
class IMaterialAsset;
class IModelAsset;
class IModelCombinationAsset;
class IRenderAnimAsset;

AssetDBRow_Mesh_v_0 AssetToDBRow_Mesh_v_0(const IMeshAsset* InAsset);
AssetDBRow_Tex_v_0 AssetToDBRow_Tex_v_0(const ITextureAsset* InAsset);
AssetDBRow_Mtl_DefaultPBR_v_0 AssetToDBRow_Mtl_DefaultPBR_v_0(const IMaterialAsset* InAsset);
AssetDBRow_Mdl_v_0 AssetToDBRow_Mdl_v_0(const IModelAsset* InAsset);
AssetDBRow_Mdlc_v_0 AssetToDBRow_Mdlc_v_0(const IModelCombinationAsset* InAsset);