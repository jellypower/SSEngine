#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"


// Common Render Asset Names
struct CRAN
{
	// EngineDefaultAssets
	constexpr static const utf16* NS_DEFAULT_ASSET = L"EngineDefaultAssets"; // NameSpace Default Asset

	constexpr static const utf16* WHITE_TEX = L"EngineDefaultAssets/WHITE.tex";
	constexpr static const utf16* BLACK_TEX = L"EngineDefaultAssets/BLACK.tex";
	constexpr static const utf16* EMPTY_TEX = L"EngineDefaultAssets/EMPTY.tex";
	constexpr static const utf16* EMPTYNORMAL_TEX = L"EngineDefaultAssets/EMPTYNORMAL.tex";

	constexpr static const utf16* EMPTY_PBR_MTL = L"EngineDefaultAssets/EMPTY.mtl";

	constexpr static const utf16* ARROW_MESH = L"EngineDefaultAssets/Arrow/Arrow.mesh";


	// RuntimeCreationAssets
	constexpr static const utf16* NS_RUNTIME_CREATION = L"__RUNTIME_CREATION__"; // NameSpace Runtime Creation

	constexpr static const utf16* CUBE1M_MESH = L"__RUNTIME_CREATION__/Cube1m.mesh";
	constexpr static const utf16* SPHERE1M_MESH = L"__RUNTIME_CREATION__/Sphere1m.mesh";

	constexpr static const utf16* CUBE1M_MDL = L"__RUNTIME_CREATION__/Cube1m.mdl";
	constexpr static const utf16* SPHERE1M_MDL = L"__RUNTIME_CREATION__/Sphere1m.mdl";


	// EngineDefaultRefernce
	constexpr static const utf16* AR_COMMON = L"__COMMON_ASSET_REFERENCER__";

};