#include "pch.h"
#include "SASSweepAndPrune.h"

#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"

#include "SSEngineDefault/Public/SSContainer/ContainerUtil/ContainerUtil.h"

SASSweepAndPrune::SASSweepAndPrune() :
	_SAPList(1024),
	_EnterPendingItems(32),
	_ExitPendingItems(32),
	_UpdatePendingItems(32)
{
}

ESASType SASSweepAndPrune::GetSASType() const
{
	return ESASType::SweepAndPrune;
}

bool SASSweepAndPrune::IsAnyInstanceExists() const
{
	if (_SAPList.GetSize() > 0 ||
		_EnterPendingItems.GetSize() > 0)
	{
		return true;
	}

	return false;
}

void SASSweepAndPrune::AddCollInstance(ICollInstanceBase* InCollInstance)
{
	_EnterPendingItems.PushBack(InCollInstance);
}

void SASSweepAndPrune::RemoveCollInstance(ICollInstanceBase* InCollInstance)
{
	_ExitPendingItems.PushBack(InCollInstance);
}

void SASSweepAndPrune::AddUpdateNeededCollInstance(ICollInstanceBase* InCollInstance)
{
	_UpdatePendingItems.PushBack(InCollInstance);
}

void SASSweepAndPrune::FinalizePendingInstances()
{
	UpdateSAPStructure();
}

void SASSweepAndPrune::QueryCollidableWith(SS::PooledList<ICollInstanceBase*>& OutList, ICollInstanceBase* CollTarget) const
{
	// SAP는 쿼리에 알맞지 않다
}

void SASSweepAndPrune::UpdateSAPStructure()
{
	for (ICollInstanceBase* ICIToRemove : _ExitPendingItems)
	{
		RemoveFromSAPList(ICIToRemove);
	}

	for (ICollInstanceBase* ICIToAdd : _EnterPendingItems)
	{
		AddToSAPList(ICIToAdd);
	}

	const int32 SAPCnt = _SAPList.GetSize();
	for (int32 i = 1; i < SAPCnt; i++)
	{
		float Key = ExtractSAPElementAxisValue(_SAPList[i]);
		

		int32 j = i - 1;
		while (j >= 0 && ExtractSAPElementAxisValue(_SAPList[j]) > Key)
		{
			SAPElement Prev = _SAPList[j];
			SAPElement Cur = _SAPList[j + 1];

			if (Prev.bStart)
			{
				if (Cur.bStart)
				{
					_CIProxies[Prev.ProxyIdx].SAPListStartIdx = j + 1;
					_CIProxies[Cur.ProxyIdx].SAPListStartIdx = j;
				}
				else
				{
					_CIProxies[Prev.ProxyIdx].SAPListStartIdx = j + 1;
					_CIProxies[Cur.ProxyIdx].SAPListEndIdx = j;
				}
			}
			else
			{
				if (Cur.bStart)
				{
					_CIProxies[Prev.ProxyIdx].SAPListEndIdx = j + 1;
					_CIProxies[Cur.ProxyIdx].SAPListStartIdx = j;
				}
				else
				{
					_CIProxies[Prev.ProxyIdx].SAPListEndIdx = j + 1;
					_CIProxies[Cur.ProxyIdx].SAPListEndIdx = j;
				}
			}

			_SAPList[j] = Cur;
			_SAPList[j + 1] = Prev;

			j--;
		}
	}


	int32 ExitPendingItemCnt = _ExitPendingItems.GetSize();
	if (ExitPendingItemCnt > 0)
	{
		int32 CurSAPSize = _SAPList.GetSize();
		_SAPList.Resize(CurSAPSize - ExitPendingItemCnt * 2);
	}

	_ExitPendingItems.Clear();
	_EnterPendingItems.Clear();
	_UpdatePendingItems.Clear();
}

void SASSweepAndPrune::RemoveFromSAPList(ICollInstanceBase* ICIToRemove)
{
//	const int32 ProxyIdxToSwap = ICIToRemove->GetSASProxyIdx();
//	const CIProxy ProxyItemToRemove = _CIProxies[ProxyIdxToSwap];
//	if (ProxyItemToRemove.Coll != ICIToRemove) // Validation
//	{
//		SS_INTERRUPT();
//		return;
//	}
//
//	ICIToRemove->SetSASProxyIdx(-1);
//
//	const int32 LastIdx = _CIProxies.GetSize() - 1;
//	_CIProxies[ProxyIdxToSwap] = _CIProxies[LastIdx];
//	CIProxy& SwapedProxy = _CIProxies[ProxyIdxToSwap];
//	SwapedProxy.Coll->SetSASProxyIdx(ProxyIdxToSwap);
//	_SAPList[SwapedProxy.SAPListStartIdx].ProxyIdx = ProxyIdxToSwap;
//	_SAPList[SwapedProxy.SAPListEndIdx].ProxyIdx = ProxyIdxToSwap;
//	_CIProxies.PopBack();
//
//
//	SAPElement& SAPStartToRemove = _SAPList[ProxyItemToRemove.SAPListStartIdx];
//	SS_ASSERT(SAPStartToRemove.ProxyIdx == ProxyIdxToSwap);
//	SAPStartToRemove.ProxyIdx = -1; // 삽입 정렬시에 어레이의 끝쪽에 몰리도록 유도
//
//	SAPElement& SAPEndToRemove = _SAPList[ProxyItemToRemove.SAPListEndIdx];
//	SS_ASSERT(SAPEndToRemove.ProxyIdx == ProxyIdxToSwap);
//	SAPEndToRemove.ProxyIdx = -1;
//
//	ICIToRemove->OnExitTheSAS();
}

void SASSweepAndPrune::AddToSAPList(ICollInstanceBase* ICIToAdd)
{
//	const int32 NewProxyIdx = _CIProxies.GetSize();
//	const int32 SAPStartIdx = _SAPList.GetSize();
//
//	_SAPList.PushBack({ NewProxyIdx, true });
//	_SAPList.PushBack({ NewProxyIdx, false });
//
//	_CIProxies.PushBack({ ICIToAdd, SAPStartIdx, SAPStartIdx + 1 });
//	ICIToAdd->SetSASProxyIdx(NewProxyIdx);
//
//	ICIToAdd->OnEnterTheSAS(this);
}

float SASSweepAndPrune::ExtractSAPElementAxisValue(SAPElement InElement) const
{
	if (InElement.ProxyIdx < 0)
	{
		return FLT_MAX; // 리스트에서 제외해야 할 놈
	}

	ICollInstanceBase* Coll = _CIProxies[InElement.ProxyIdx].Coll; // 삽입 정렬시에 어레이의 끝쪽에 몰리도록 유도
	const AABBBox& CollBBox = Coll->GetBBox();
	return InElement.bStart ? CollBBox.Min.X : CollBBox.Max.X;
}