#pragma once

constexpr utf16 ALL_TEXTURE_QUERY[] = L"SELECT * from Textures";
constexpr utf16 ALL_MTL_QUERY[] = L"SELECT * from Mtl_DefaultPBR";
constexpr utf16 ALL_MDL_QUERY[] = L"SELECT * from Models";
constexpr utf16 ALL_MESH_QUERY[] = L"SELECT * from Meshes";
constexpr utf16 ALL_MDLC_QUERY[] = L"SELECT * from ModelCombinations";



constexpr utf16 SAVE_Mesh_v_0_QUERY[] =
L"INSERT INTO Meshes(AssetName, AssetPath, LastUpdateTime)"
L"VALUES(? , ? , ? )"

L"ON CONFLICT(AssetName) DO UPDATE SET "

L"AssetPath = excluded.AssetPath,"
L"LastUpdateTime = excluded.LastUpdateTime "

L"WHERE LastUpdateTime < excluded.LastUpdateTime;";


constexpr utf16 SAVE_DefaultPBR_Mtl_v_0_QUERY[] =
L"INSERT INTO Mtl_DefaultPBR("
L"AssetName, AssetPath, LastUpdateTime, "
L"BaseColor_r, BaseColor_g, BaseColor_b, "
L"Emissive_r, Emissive_g, Emissive_b, "
L"NormalTexScale, Metallic, Roughness, "
L"BaseColor_Tex_ID, Normal_Tex_ID, Metallic_Tex_ID, Emissive_Tex_ID, Occlusion_Tex_ID) "

L"VALUES( "
L"? , ? , ? , "
L"? , ? , ? , "
L"? , ? , ? , "
L"? , ? , ? , "
L"? , ? , ? , ? , ? ) "
L"ON CONFLICT(AssetName) "
L"DO UPDATE SET "
L"AssetPath = excluded.AssetPath, LastUpdateTime = excluded.LastUpdateTime, "
L"BaseColor_r = excluded.BaseColor_r, BaseColor_g = excluded.BaseColor_g, BaseColor_b = excluded.BaseColor_b, "
L"Emissive_r = excluded.Emissive_r, Emissive_g = excluded.Emissive_g, Emissive_b = excluded.Emissive_b, "

L"NormalTexScale = excluded.NormalTexScale, Metallic = excluded.Metallic, Roughness = excluded.Roughness, "

L"BaseColor_Tex_ID = excluded.BaseColor_Tex_ID, "
L"Normal_Tex_ID = excluded.Normal_Tex_ID, "
L"Metallic_Tex_ID = excluded.Metallic_Tex_ID, "
L"Emissive_Tex_ID = excluded.Emissive_Tex_ID, "
L"Occlusion_Tex_ID = excluded.Occlusion_Tex_ID "

L"WHERE LastUpdateTime < excluded.LastUpdateTime; ";






constexpr utf16 SAVE_Mdl_v_0_QUERY[] = 
L"INSERT INTO Models(AssetName, AssetPath, LastUpdateTime, MeshName, MtlNames) "
L"VALUES (?, ?, ?, ?, ?) "

L"ON CONFLICT(AssetName) DO UPDATE SET "

L"AssetPath = excluded.AssetPath, "
L"LastUpdateTime = excluded.LastUpdateTime,"
L"MeshName = excluded.MeshName,"
L"MtlNames = excluded.MtlNames "

L"WHERE LastUpdateTime < excluded.LastUpdateTime;";





constexpr utf16 SAVE_Mdlc_v_0_QUERY[] = 
L"INSERT INTO ModelCombinations(AssetName, AssetPath, LastUpdateTime) "
L"VALUES (?, ?, ?) "

L"ON CONFLICT(AssetName) DO UPDATE SET "

L"AssetPath = excluded.AssetPath,"
L"LastUpdateTime = excluded.LastUpdateTime "

L"WHERE LastUpdateTime < excluded.LastUpdateTime";