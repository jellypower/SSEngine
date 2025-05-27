#include "SObject/Public/SComponentBase.h"
#include "SObject/Public/SGameObject.h"

void SComponentBase::InitComponentWithParent(SGameObject* InParent)
{
	if (_Parent != nullptr)
	{
		SS_ASSERT_MSG(false, L"Canging Component's parent is impossible.");
		return;
	}

	_Parent = InParent;
	if (_Parent->GetIsHierarchyInitialized())
	{
		PostConstructHierarchy();
	}
}
