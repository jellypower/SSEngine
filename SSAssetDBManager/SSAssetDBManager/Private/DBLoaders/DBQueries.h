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