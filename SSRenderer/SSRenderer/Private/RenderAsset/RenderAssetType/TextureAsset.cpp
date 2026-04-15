#include "TextureAsset.h"

#include "SSGAL/Public/GALRenderAsset/GALTextureAssetWrapperBase.h"

#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"

TextureAsset::TextureAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, ETextureType InType, time_t LastUpdateTime)
{
	_DBNameSpace = InDBNameSpace;
	_assetName = InAssetName;
	_assetPath = InAssetPath;
	_Type = InType;
	_LastUpdateTime = LastUpdateTime;
}

EAssetType TextureAsset::GetAssetType() const
{
	return ThisAssetType;
}

void TextureAsset::AddAssetReference(const AssetInstanceReferencer& Referencer)
{
	for (int32 i = 0; i < _AssetInstanceReferencers.GetSize(); i++)
	{
		if (_AssetInstanceReferencers[i] == Referencer)
		{
			SS_ASSERT_MSG(false, L"Reference already exists.");
			return;
		}
	}

	int32 PrevReferencerCnt = _AssetInstanceReferencers.GetSize();
	_AssetInstanceReferencers.PushBack(Referencer);

	if (PrevReferencerCnt == 0)
	{
		SSRenderer* Renderer = (SSRenderer*)g_Renderer;
		Renderer->AddGALStateChangedAsset(this);
	}
}

void TextureAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
{
	bool bReferencerEverRemoved = false;

	for (int32 i = 0; i < _AssetInstanceReferencers.GetSize(); i++)
	{
		if (_AssetInstanceReferencers[i] == ReferencerName)
		{
			_AssetInstanceReferencers.RemoveAtAndFillLast(i);
			bReferencerEverRemoved = true;
			break;
		}
	}
	if (bReferencerEverRemoved == false)
	{
		SS_ASSERT_MSG(false, L"Reference do not exist.");
		return;
	}

	int32 ReferencerCnt = _AssetInstanceReferencers.GetSize();
	if (ReferencerCnt == 0)
	{
		SSRenderer* Renderer = (SSRenderer*)g_Renderer;
		Renderer->AddGALStateChangedAsset(this);
	}
}

void TextureAsset::BindAssetManager(IAssetManager* InAssetManager)
{
	_BoundAssetManager = InAssetManager;
}

void TextureAsset::ReleaseGALData()
{
	delete _GALTextureAsset;
	_GALTextureAsset = nullptr;
}

ETextureType TextureAsset::GetTextureType() const
{
	return _Type;
}
