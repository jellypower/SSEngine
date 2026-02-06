#include "MaterialAsset.h"

#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataBase.h"

#include "SSGAL/Public/GALRenderAsset/GALMaterialAssetWrapperBase.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"

#include "SSEngineDefault/Public/SSContainer/ContainerUtil/ContainerUtil.h"

MaterialAsset::MaterialAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	_DBNameSpace = InDBNameSpace;
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
	if (PrevReferencerCnt == 0)
	{
		ApplyMtlDataModify();


		AssetInstanceReferencer ThisAssetReferencer = MakeThisAssetReferencer();
		for (ITextureAsset* TexItem : _ReferencingTextures)
		{
			TexItem->AddAssetReference(ThisAssetReferencer);
		}


		SSRenderer* Renderer = (SSRenderer*)g_Renderer;
		Renderer->AddGALStateChangedAsset(this);
	}

	_AssetInstanceReferencers.PushBack(Referencer);

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

	int32 ReferencerCnt = GetAssetInstanceReferenceCnt();
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

void MaterialAsset::ApplyMtlDataModify()
{
	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	if (AssetManager == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	const bool bIsThisAssetAlive = GetAssetInstanceReferenceCnt() > 0;
	const AssetInstanceReferencer ThisReferencer = MakeThisAssetReferencer();

	// 레퍼런스를 잡고있는 경우 메테리얼이 변경되면 해당 메테리얼이 가지고 있는 텍스처의 레퍼런스도 바뀌어야 한다.
	// 텍스처 레퍼런스를 변경하기 위해 기존에 가지고 있던 레퍼런스를 다 날리고 다시 구축한다.
	if (bIsThisAssetAlive)
	{
		for (ITextureAsset* TexItem : _ReferencingTextures)
		{
			TexItem->RemoveAssetReference(ThisReferencer);
		}
	}


	// 레퍼런스하고있는 텍스처의 리스트들을 재구축한다.
	_ReferencingTextures.Clear();
	if (_MtlData->_Type == EMaterialType::DefaultPBR)
	{
		MtlDataDefaultPBR* PbrMtlData = static_cast<MtlDataDefaultPBR*>(_MtlData);

		for (int32 i = 0; i < (int32)EDefaultPBRMatTexTypes::Count; i++)
		{
			SS::SHasherW NewReferencingTexName = PbrMtlData->_TextureAssetNames[i];
			if (NewReferencingTexName.IsEmpty())
			{
				SS_ASSERT(false);
				PbrMtlData->_CachedTextureRefs[i] = nullptr;
				continue;
			}

			ITextureAsset* NewReferencingTexture = AssetManager->FindAssetByName<ITextureAsset>(NewReferencingTexName);
			if (NewReferencingTexture == nullptr)
			{
				SS_ASSERT(false);
				PbrMtlData->_CachedTextureRefs[i] = nullptr;
				continue;
			}

			PbrMtlData->_CachedTextureRefs[i] = NewReferencingTexture;
			ListPushBackUnique(_ReferencingTextures, NewReferencingTexture);
		}
	}
	else
	{
		SS_ASSERT(false);
	}


	// 새로운 텍스쳐 레퍼런스들을 전부 추가한다.
	if (bIsThisAssetAlive)
	{
		for (ITextureAsset* TexItem : _ReferencingTextures) 
		{
			TexItem->AddAssetReference(ThisReferencer);
		}

		// GAL 상태를 변화하도록 유도한다.
		SSRenderer* Renderer = (SSRenderer*)g_Renderer; 
		Renderer->AddGALStateChangedAsset(this);
	}
}