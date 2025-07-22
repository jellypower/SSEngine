#include "MaterialAsset.h"

#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataBase.h"

#include "SSGAL/Public/GALRenderAsset/GALMaterialAssetWrapperBase.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"

MaterialAsset::MaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	_assetName = InAssetName;
	_assetPath = InAssetPath;
}

MaterialAsset::~MaterialAsset()
{
	SS_ASSERT(_MtlData != nullptr);
	delete _MtlData;
}

EAssetType MaterialAsset::GetAssetType() const
{
	return ThisAssetType;
}

void MaterialAsset::AddAssetReference(const AssetInstanceReferencer& Referencer)
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
		AssetInstanceReferencer ThisAssetReferencer = MakeThisAssetReferencer();
		for (ITextureAsset* TexItem : _ReferencingTextures)
		{
			TexItem->AddAssetReference(ThisAssetReferencer);
		}

		SSRenderer* Renderer = (SSRenderer*)g_Renderer;
		Renderer->AddGALStateChangedAsset(this);
	}
}

void MaterialAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
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
		SS_ASSERT_MSG(false, L"Reference does not exist.");
		return;
	}

	int32 ReferencerCnt = _AssetInstanceReferencers.GetSize();
	if (ReferencerCnt == 0)
	{
		AssetInstanceReferencer ThisAssetReferencer = MakeThisAssetReferencer();
		for (ITextureAsset* TexItem : _ReferencingTextures)
		{
			TexItem->RemoveAssetReference(ThisAssetReferencer);
		}

		SSRenderer* Renderer = (SSRenderer*)g_Renderer;
		Renderer->AddGALStateChangedAsset(this);
	}

}

void MaterialAsset::ReleaseGALData()
{
	delete _GALMaterialAsset;
	_GALMaterialAsset = nullptr;
}

void MaterialAsset::NotifyMtlDataModified()
{
	bool bIsMaterialInstantiated = _AssetInstanceReferencers.GetSize() > 0;
	AssetInstanceReferencer ThisReferencer = MakeThisAssetReferencer();

	if (bIsMaterialInstantiated) // 레퍼런스를 잡고있는 경우 메테리얼이 변경되면
	{
		for (ITextureAsset* TexItem : _ReferencingTextures) // 기존에 잡아놨던 텍스쳐 레퍼런스를 전부 날린다.
		{
			TexItem->RemoveAssetReference(ThisReferencer);
		}
	}

	_ReferencingTextures.Clear();
	if (_MtlData->_Type == EMaterialType::DefaultPBR)
	{
		const MtlDataDefaultPBR* PbrMtlData = (MtlDataDefaultPBR*)_MtlData;

		for (ITextureAsset* NewReferencingTex : PbrMtlData->_Textures)
		{
			bool bAlreadyReferencing = false;
			for (ITextureAsset* ReferencingTextureItem : _ReferencingTextures)
			{
				if (NewReferencingTex == ReferencingTextureItem)
				{
					bAlreadyReferencing = true;
					break;
				}
			}

			if (bAlreadyReferencing == false && NewReferencingTex != nullptr)
			{
				_ReferencingTextures.PushBack(NewReferencingTex); // ReferencingTexture 를 재구축해준다.
			}
		}
	}
	else
	{
		SS_ASSERT(false);
	}

	if (bIsMaterialInstantiated)
	{
		for (ITextureAsset* TexItem : _ReferencingTextures) // 새로운 텍스쳐 레퍼런스들을 전부 추가한다.
		{
			if (TexItem != nullptr)
			{
				TexItem->AddAssetReference(ThisReferencer);
			}
		}
	}


	if (bIsMaterialInstantiated)
	{
		SSRenderer* Renderer = (SSRenderer*)g_Renderer; // GAL 상태를 변화하도록 유도한다.
		Renderer->AddGALStateChangedAsset(this);
	}
}