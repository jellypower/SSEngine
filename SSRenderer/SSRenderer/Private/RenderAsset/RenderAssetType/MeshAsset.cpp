#include "MeshAsset.h"

#include "SSGAL/Public/GALRenderAsset/GALMeshAssetWrapperBase.h"
#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

MeshAsset::MeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	_assetName = InAssetName;
	_assetPath = InAssetPath;
}

EAssetType MeshAsset::GetAssetType() const
{
	return ThisAssetType;
}

void MeshAsset::AddAssetReference(const AssetInstanceReferencer& Referencer)
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

void MeshAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
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
		SSRenderer* Renderer = (SSRenderer*)g_Renderer;
		Renderer->AddGALStateChangedAsset(this);
	}

}

void MeshAsset::ReleaseSystemData()
{
	delete _MeshRawData;
	_MeshRawData = nullptr;
}

void MeshAsset::ReleaseGALData()
{
	delete _GALMeshAsset;
	_GALMeshAsset = nullptr;
}
