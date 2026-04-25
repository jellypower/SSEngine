#include "RenderWorld.h"

#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderInstance/GALRWMetaData.h"

#include "SSRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderCommon/SSRenderUtilFuncs.h"
#include "SSRenderer/Public/RenderInstance/IRICubeMap.h"
#include "SSRenderer/Public/RenderInstance/Light/IRenderLight.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"
#include "SSRenderer/Public/RenderInstance/IRISkinnedMesh.h"


RenderWorld::RenderWorld(const utf16* InWorldName) :
	_RenderInstanceByHashCode(RENDERWORLD_HASHMAP_SIZE, RENDERWORLD_BUCKET_CAPACITY)
{
	_RenderWorldName = InWorldName;
}

RenderWorld::~RenderWorld()
{
	SS_ASSERT(_RenderInstanceByHashCode.GetCnt() == 0);


	int32 CurFrameMod = RenderFrameInfo::GetFrameMod();

	for (int32 Offset = 0; Offset < GAL_NESTED_FRAME_CNT; Offset++)
	{
		const int32 ItemIdx =
			(CurFrameMod - Offset // CurFrameMod가 N이라고 하면 N-1번째 아이템은 CurFrame-1번째에 사용했던 녀석
				+ GAL_NESTED_FRAME_CNT) // CurFrameMod - Offset 값이 0보다 작을 수 있기 때문에 더해줌
			% GAL_NESTED_FRAME_CNT; // 그리고 다시 나눠줌

		if (_GALMetadata[ItemIdx] == nullptr)
		{
			continue;
		}

		const int32 DestroyDelay =
			(ItemIdx + GAL_NESTED_FRAME_CNT) % // 중첩된 프레임(ex. 2frame) 뒤에 지운다.
			DEFERRED_DESTROY_MOD; // 위 값도 리밋을 넘을 수 있으니까 모듈러 한 번 더 해줌.

		g_Renderer->ReserveDestroyGALRW(_GALMetadata[ItemIdx], DestroyDelay);
		_GALMetadata[ItemIdx] = nullptr;
	}
}


bool RenderWorld::IsAnyInstanceRemainInWorld() const
{return _RenderInstanceByHashCode.GetCnt() != 0;
}

SS::SHasherW RenderWorld::GetWorldName() const
{
	return _RenderWorldName;
}

void RenderWorld::AddToWorld(IRenderInstance* InRenderInstance)
{
	SObjHashCode GameObjectHashCode = InRenderInstance->GetGameObjectID();
	if (_RenderInstanceByHashCode.Find(GameObjectHashCode) != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstanceByHashCode.Add(GameObjectHashCode, InRenderInstance);
	InRenderInstance->OnEnterTheRenderWorldXXX(this);
}


void RenderWorld::RemoveRenderInstanceFromWorld(SObjHashCode RenderInstanceIDToRemove)
{
	IRenderInstance** ppRenderInstance = _RenderInstanceByHashCode.Find(RenderInstanceIDToRemove);
	if (ppRenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	IRenderInstance* RenderInstanceToRemove = *ppRenderInstance;
	if (RenderInstanceToRemove == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstanceByHashCode.Remove(RenderInstanceIDToRemove);
	RenderInstanceToRemove->OnExitFromRenderWorldXXX();
}

GALRWMetaData* RenderWorld::GetGALMetadata(int32 FrameMod) const
{
	return _GALMetadata[FrameMod];
}

void RenderWorld::InjectGALMetadataXXX(GALRWMetaData* InMetadata, int32 FrameMod)
{
	_GALMetadata[FrameMod] = InMetadata;
}