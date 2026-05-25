#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/ContentBase/SWorld.h"

#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"
#include "SSCollision/Public/RigidBody/IRigidBodyCustomSim.h"
#include "SSCollision/Public/RigidBody/IRigidBodyDynamic.h"

#include "SSContentsBase/Private/AnimWorker/AnimWorkerBase.h"
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SRigidBodyBaseComponent.h"
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"

#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"
#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"


SWorld::SWorld() :
	_ObjectsByHashCode(WORLD_OBJECTMAP_HASHMAP_SIZE, WORLD_OBJECTMAP_HASHBUCKET_SIZE),
	_FrameProcessComponents(WORLD_OBJECTMAP_HASHMAP_SIZE, WORLD_OBJECTMAP_HASHBUCKET_SIZE),
	_TransformCommitNeededObjs(TRANSFORM_UPDATE_HASHMAP_SIZE, TRANSFORM_UPDATE_HASHBUCKET_SIZE)
{
}

SWorld::~SWorld()
{
}

void SWorld::PostConstruct()
{
	_WorldRootObject = NewSObject<SGameObject>(L"WorldRoot");
	SObjHashCode WorldHashCode = GetHashCode();
	AddWorldRootObject(_WorldRootObject);
}


void SWorld::InitializeWorld(IRenderWorld* InRenderWorld, ICollisionWorld* InCollWorld)
{
	_RenderWorld = InRenderWorld;
	_CollWorld = InCollWorld;

	_AnimWorker = DBG_NEW AnimWorkerBase(this);
}

bool SWorld::DEBUG_Validate_TransformCommit() const
{
	SCOPE_PROFILE(DEBUG_Validate_TransformCommit);
	for (SS::pair<SObjHashCode, SGameObject*> Item : _ObjectsByHashCode)
	{
		const SGameObject* GO = Item.second;
		if (GO->IsTransformCommitReserved())
		{
			SGameObject* const* Found = _TransformCommitNeededObjs.Find(Item.first);

			if (Found == nullptr)
			{
				// 게임오브젝트에는 트랜스폼이 커밋됐다고 나오는데 맵에선 없으면 안됨.
				return false;
			}
		}
	}

	return true;
}

void SWorld::PerFrameContents()
{
	const float DeltaTime = SSFrameInfo::GetDeltaTime();

	for (SS::pair<SObjHashCode, SComponentBase*>& ComponentPairItem : _FrameProcessComponents)
	{
		SComponentBase* ComponentItem = ComponentPairItem.second;
		ComponentItem->PerFrame(_TimeScale * DeltaTime);
	}
}

void SWorld::PerFrameAnim()
{
	const float DeltaTime = SSFrameInfo::GetDeltaTime();
	_AnimWorker->BeginUpdateAnimation(DeltaTime * _TimeScale);
	_AnimWorker->EndUpdateAnimation();
}

void SWorld::PerFrameCollision()
{
	const float SmoothDeltaTime = SSFrameInfo::GetSmoothDeltaTime();

	{
		SCOPE_PROFILE(Simulate);
		_CollWorld->OnBeginSimulation();
		_CollWorld->SimulateMovement(SmoothDeltaTime * _TimeScale);
		_CollWorld->OnEndSimulation();
	}


	{
		SCOPE_PROFILE(Apply);
		const SS::HashMap<SObjHashCode, IRigidBodyBase*>& StaticRigidBodies = _CollWorld->GetStaticRigidBodies();
		for (const SS::pair<SObjHashCode, IRigidBodyBase*>& RigidBodyItem : StaticRigidBodies)
		{
			if (RigidBodyItem.second->IsTransformModifiedOnThisTick() == false)
			{
				continue;
			}

			SRigidBodyBaseComponent* RigidBodyIComp = static_cast<SRigidBodyBaseComponent*>(RigidBodyItem.first.GetSObject());
			RigidBodyIComp->PostCollision_SyncTransform();
		}

		const auto& DynamicRigidBodies = _CollWorld->GetDynamicRigidBodies();
		for (const SS::pair<SObjHashCode, IRigidBodyDynamic*>& RigidBodyItem : DynamicRigidBodies)
		{
			if (RigidBodyItem.second->IsTransformModifiedOnThisTick() == false)
			{
				continue;
			}

			SRigidBodyBaseComponent* RigidBodyIComp = static_cast<SRigidBodyBaseComponent*>(RigidBodyItem.first.GetSObject());
			RigidBodyIComp->PostCollision_SyncTransform();
		}

		const SS::HashMap<SObjHashCode, IRigidBodyCustomSim*>& CustomRigidBodies = _CollWorld->GetCustomSimBodies();
		for (const SS::pair<SObjHashCode, IRigidBodyCustomSim*>& RigidBodyItem : CustomRigidBodies)
		{
			if (RigidBodyItem.second->IsTransformModifiedOnThisTick() == false)
			{
				continue;
			}

			SRigidBodyBaseComponent* RigidBodyIComp = static_cast<SRigidBodyBaseComponent*>(RigidBodyItem.first.GetSObject());
			RigidBodyIComp->PostCollision_SyncTransform();
		}
	}

}

bool SWorld::IsAnyObjectRemainInWorld() const
{
	for (const SS::pair<SObjHashCode, SGameObject*>& item : _ObjectsByHashCode)
	{
		if (item.second != nullptr)
		{
			return true;
		}
	}
	return false;
}

void SWorld::CleanupWorld()
{
	int32 ChildCnt = _WorldRootObject->GetChildCnt();

	SS::PooledList<SGameObject*> RootedObjs;
	RootedObjs.Reserve(ChildCnt);

	for (int32 i = 0; i < ChildCnt; i++)
	{
		SGameObject* ChildItem = _WorldRootObject->GetChild(i);
		RemoveFromWorld(ChildItem); // 일단 전부 World로부터 빼내기
		RootedObjs.PushBack(ChildItem); 
	}

	if (_AnimWorker != nullptr)
	{
		delete _AnimWorker;
		_AnimWorker = nullptr;
	}


	bool Remain = _RenderWorld->IsAnyInstanceRemainInWorld();
	SS_ASSERT(Remain == false);

	_CollWorld->FinalizeCollWorld();
	delete _CollWorld;
	_CollWorld = nullptr;

	delete _RenderWorld;
	_RenderWorld = nullptr;


	for (int32 i = 0; i < ChildCnt; i++)
	{
		SGameObjectConstructor::DestroyAll(RootedObjs[i]); // 처리 완료하고 Destroy
	}

	DelSObject(_WorldRootObject);
	_WorldRootObject = nullptr;
}

void SWorld::ProcessTransformCommit()
{
	int64 PC1;
	int64 PC2;
	int64 PF;
	double eTime;
	PC1 = GetPerofrmanceCounter();

	uint64 CurFrameCnt = SSFrameInfo::GetFrameCnt();
	EFramePhase CurFramePhase = SSFrameInfo::GetFramePhase();


	for (SS::pair<SObjHashCode, SGameObject*>& PairItem : _TransformCommitNeededObjs)
	{
		SGameObject* TransformCommitStartObject = PairItem.second;
		if (TransformCommitStartObject->GetTransformCommittedFrameCnt() == CurFrameCnt &&
			TransformCommitStartObject->GetTransformCommitedPhase() == CurFramePhase)
		{
			continue;
		}


		// FindTransformCommitStartObject
		{
			SGameObject* CommitStartAncestorItem = TransformCommitStartObject->GetParent();
			while (CommitStartAncestorItem != nullptr)
			{
				if (CommitStartAncestorItem->IsTransformCommitReserved())
				{
					TransformCommitStartObject = CommitStartAncestorItem;
				}

				CommitStartAncestorItem = CommitStartAncestorItem->GetParent();
			}
		}

		SGameObject* TransformCommitStartParent = TransformCommitStartObject->GetParent();
		XMMATRIX ParentWorldTransform;
		Quaternion ParentWorldRotation;

		if (TransformCommitStartParent == nullptr)
		{
			ParentWorldTransform = XMMatrixIdentity();
		}
		else
		{
			ParentWorldTransform = TransformCommitStartParent->CalcWorldTransformMatrix();
			ParentWorldRotation = TransformCommitStartParent->CalcWorldRot();
		}

		TransformCommitStartObject->CommitTransform(ParentWorldTransform, ParentWorldRotation);
	}


	_TransformCommitNeededObjs.Clear();

	PC2 = GetPerofrmanceCounter();
	PF = GetPerformanceFrequency();
	eTime = (PC2 - PC1) / (double)PF;
	int a = 0;
}

void SWorld::AddTransformCommitNeededObj(SGameObject* InObj)
{
	SObjHashCode WorldHashCode = GetHashCode();
	if (WorldHashCode != InObj->GetIncludedWorldHash())
	{
		SS_INTERRUPT();
	}

	SObjHashCode InObjHashCode = InObj->GetHashCode();
	SGameObject** ppFoundObj = _TransformCommitNeededObjs.Find(InObjHashCode);
	if (ppFoundObj != nullptr)
	{
		return; // 부모 오브젝트의 위치가 업데이트 되면서 자식 오브젝트를 포함시켰으면 이미 존재할 수도 있음
	}

	_TransformCommitNeededObjs.Add(InObjHashCode, InObj);
}

void SWorld::AddToWorld(SGameObject* InNewWorldObject, SGameObject* ParentObject)
{
	if (InNewWorldObject->GetIncludedWorldHash() != nullptr)
	{
		SS_ASSERT_MSG(false, L"To add to world, GameObject must not be included in any world.");
		return;
	}


	if (ParentObject != nullptr)
	{
		if (GetHashCode() != ParentObject->GetIncludedWorldHash())
		{
			SS_ASSERT_MSG(false, L"Parent must be included same world.");
			return;
		}

		AddGameObjectItem(InNewWorldObject);
		InNewWorldObject->SetParent(ParentObject);
	}
	else
	{
		AddGameObjectItem(InNewWorldObject);
		InNewWorldObject->SetParent(_WorldRootObject);
	}

	int32 ChildCnt = InNewWorldObject->GetChildCnt();
	for (int32 i = 0; i < ChildCnt; i++)
	{
		SGameObject* ChildObj = InNewWorldObject->GetChild(i);
		AddToWorld_Recursion(ChildObj);
	}
}

void SWorld::RemoveFromWorld(SGameObject* InObjectToRemove)
{
	RemoveFromWorld_Recursion(InObjectToRemove);
}

void SWorld::AddToWorld_Recursion(SGameObject* InNewObject)
{
	SGameObject** ppFoundGameObject = _ObjectsByHashCode.Find(InNewObject->GetHashCode());

	if (ppFoundGameObject != nullptr && (*ppFoundGameObject) != nullptr)
	{
		SS_ASSERT_MSG(false, L"Already smae object exists.");
		return;
	}

	// ==============================
	AddGameObjectItem(InNewObject);
	// ==============================

	int32 ChildCnt = InNewObject->GetChildCnt();
	for (int32 i = 0; i < ChildCnt; i++)
	{
		SGameObject* ChildObj = InNewObject->GetChild(i);
		AddToWorld_Recursion(ChildObj);
	}
}

void SWorld::RemoveFromWorld_Recursion(SGameObject* InObjectToRemove)
{
	int32 ChildCnt = InObjectToRemove->GetChildCnt();
	for (int32 i = ChildCnt - 1; i >= 0; i--)
	{
		SGameObject* ChildObj = InObjectToRemove->GetChild(i);
		RemoveFromWorld_Recursion(ChildObj);
	}

	// ==============================
	RemoveGameObjectItem(InObjectToRemove);
	// ==============================
}


void SWorld::AddGameObjectItem(SGameObject* InNewObject)
{
	if (InNewObject->GetIsHierarchyInitialized() == false)
	{
		SS_ASSERT_MSG(false, L"To add to world, Hierarchy must be initialied."); 
		return;
	}

	InNewObject->OnEnterTheWorld(GetHashCode());

	_ObjectsByHashCode.Add(InNewObject->GetHashCode(), InNewObject);

	int32 CompCnt = InNewObject->GetComponentCnt();
	for (int32 i = 0; i < CompCnt; i++)
	{
		SComponentBase* CompItem = InNewObject->GetComponentByIdx(i);

		if (CompItem->ShouldProcessPerFrameInherently())
		{
			_FrameProcessComponents.Add(CompItem->GetHashCode(), CompItem);
		}
		CompItem->OnEnterTheWorld();
	}
}

void SWorld::RemoveGameObjectItem(SGameObject* InObjectToRemove)
{
	SGameObject** ppFoundGameObject = _ObjectsByHashCode.Find(InObjectToRemove->GetHashCode());
	if (ppFoundGameObject == nullptr)
	{
		SS_ASSERT_MSG(false, L"Object already removed.");
		return;
	}

	int32 CompCnt = InObjectToRemove->GetComponentCnt();
	for (int32 i = CompCnt - 1; i >= 0; i--)
	{
		SComponentBase* CompItem = InObjectToRemove->GetComponentByIdx(i);

		if (CompItem->ShouldProcessPerFrameInherently())
		{
			_FrameProcessComponents.Remove(CompItem->GetHashCode());
		}
		CompItem->OnExitTheWorld();
	}

	InObjectToRemove->OnExitTheWorld();
	_ObjectsByHashCode.Remove(InObjectToRemove->GetHashCode());
}

void SWorld::AddWorldRootObject(SGameObject* InWorldRootObject)
{
	_WorldRootObject = InWorldRootObject;
	_WorldRootObject->MarkHierarchyInitialized();
	_WorldRootObject->OnEnterTheWorld(GetHashCode());
}

void SWorld::ProcessDebugDraw(IRenderer* InRenderer)
{
	// Process Colision
	IMeshAsset* Cube = g_Renderer->GetCommonRenderAssetSet()->GetCube1mMesh();
	IMeshAsset* Sphere = g_Renderer->GetCommonRenderAssetSet()->GetSphere1mMesh();

	const SS::PooledList<CDDD_Line>& CDDDListLine = _CollWorld->GetDDDList_Line();
	for (const CDDD_Line& Item : CDDDListLine)
	{
		SRenderDebugUtil::DrawDirectionalLine(
			this,
			Item.Start,
			Item.End,
			Item.bUseDepth,
			0.3f,
			Item.Color,
			Item.Time);
	}

	const SS::PooledList<CDDD_Mesh>& CDDDListMesh = _CollWorld->GetDDDList_Mesh();
	for (const CDDD_Mesh& Item : CDDDListMesh)
	{
		IMeshAsset* MeshAsset = Cube;
		switch (Item.Type)
		{
		case ECollDebugDraw_MeshType::Point: MeshAsset = Sphere; break;
		case ECollDebugDraw_MeshType::Box: MeshAsset = Cube; break;
		case ECollDebugDraw_MeshType::Sphere: MeshAsset = Sphere; break;
		}

		SRenderDebugUtil::DrawDebugMesh(
			this,
			Item.WMatrix,
			Item.RotMatrix,
			MeshAsset,
			Item.bUseDepth,
			Item.Color,
			Item.Time
		);
	}
	
	_CollWorld->FlushDDDList();


	//
	for (int i = 0; i < _MeshDebugDrawTasks.GetSize(); i++)
	{
		InRenderer->DrawWireFrame(_MeshDebugDrawTasks[i].RenderDesc);
	}

	for (int i = 0; i < _MeshDebugDrawTasks.GetSize(); i++)
	{
		// TODO: 나중에 World별 DeltaTime으로 바꿀 수 있음
		_MeshDebugDrawTasks[i].Time -= SSFrameInfo::GetDeltaTime();

		if (_MeshDebugDrawTasks[i].Time < 0)
		{
			_MeshDebugDrawTasks.RemoveAtAndFillLast(i);
			i--;
		}
	}

	int a = 0;
}

void SWorld::DebugDrawMesh(
	const XMMATRIX& WMatrix,
	const XMMATRIX& RotMatrix,
	IMeshAsset* MeshToDraw,
	bool bUseDepth,
	const Vector4f& Color,
	float Time)
{
	TimedDebugDrawMeshDesc NewDesc;
	NewDesc.RenderDesc.WMatrix = WMatrix;
	NewDesc.RenderDesc.RotMatrix = RotMatrix;
	NewDesc.RenderDesc.MeshAsset = MeshToDraw;
	NewDesc.RenderDesc.bUseDepth = bUseDepth;
	NewDesc.RenderDesc.DrawColor = Color;
	NewDesc.Time = Time;

	_MeshDebugDrawTasks.PushBack(NewDesc);
}