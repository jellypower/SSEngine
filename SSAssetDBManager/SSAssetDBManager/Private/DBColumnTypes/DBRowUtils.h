#pragma once
#include "Rows_v_0.h"

class ITextureAsset;
class IMeshAsset;
class IMaterialAsset;
class IModelAsset;
class IModelCombinationAsset;
class IRenderAnimAsset;

AssetDBRow_Mesh_v_0 AssetToDBRow_Mesh_v_0(const IMeshAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo);
AssetDBRow_Tex_v_0 AssetToDBRow_Tex_v_0(const ITextureAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo);
AssetDBRow_Mtl_DefaultPBR_v_0 AssetToDBRow_Mtl_DefaultPBR_v_0(const IMaterialAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo);
AssetDBRow_Mdl_v_0 AssetToDBRow_Mdl_v_0(const IModelAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo);
AssetDBRow_Mdlc_v_0 AssetToDBRow_Mdlc_v_0(const IModelCombinationAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo);


///
///

//

/// <returns>
/// if passed "Resource/AssetDB/{NameSpace}/Mesh/MyDefaultMesh.apak"
/// pass pointer of "Mesh/MyDefaultMesh.apak" C_Str
///
/// if namespace is not appriopriate with this db, return NULL
/// </returns>
const utf16* CutOffNameFromFront(SS::SHasherW InPath, SS::SHasherW InNamespacePath);