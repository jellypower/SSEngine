#include "MeshAsset.h"

#include "SSGAL/Public/GALRenderAsset/GALMeshAssetWrapperBase.h"
#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

MeshAsset::MeshAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, time_t LastUpdateTime)
{
	_DBNameSpace = InDBNameSpace;
	_assetName = InAssetName;
	_assetPath = InAssetPath;
	_LastUpdateTime = LastUpdateTime;
}

MeshAsset::~MeshAsset()
{
	if (_MeshRawData != nullptr)
	{
		delete _MeshRawData;
		_MeshRawData = nullptr;
	}
}

void MeshAsset::InjectRawDataXXX(MeshRawDataBase* InRawData)
{
	_MeshRawData = InRawData;
	_CachedMeshType = _MeshRawData->GetMeshType();
	time(&_LastUpdateTime);
}

const MeshRawDataBase* MeshAsset::GetMeshRawData() const
{
	return _MeshRawData;
}

EMeshType MeshAsset::GetMeshType() const
{
	return _CachedMeshType;
}

EAssetType MeshAsset::GetAssetType() const
{
	return ThisAssetType;
}

int32 MeshAsset::GetSubMeshCnt() const
{
	if (_CachedMeshType == EMeshType::Rigid ||
		_CachedMeshType == EMeshType::Skinned)
	{
		MeshRawDataDefault* DefaultMeshRawData = (MeshRawDataDefault*)_MeshRawData;
		return DefaultMeshRawData->_VertexHeader.subMeshCnt;
	}
	else if (_CachedMeshType == EMeshType::SimpleLine)
	{
		return 1;
	}
	else
	{
		SS_ASSERT(false);
		return 0;
	}
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
		SSRenderer* Renderer = static_cast<SSRenderer*>(g_Renderer);
		Renderer->AddGALStateChangedAsset(this);
	}

}

void MeshAsset::BindAssetManager(IAssetManager* InAssetManager)
{
	_BoundAssetManager = InAssetManager;
}

void MeshAsset::ReleaseSystemData()
{
	_MeshRawData->ReleaseData();
}

void MeshAsset::ReleaseGALData()
{
	delete _GALMeshAsset;
	_GALMeshAsset = nullptr;
}
