#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"

void SComponentBase::InitComponentWithParent(SGameObject* InParent)
{
	if (_ParentGameObject != nullptr)
	{
		SS_ASSERT_MSG(false, L"Canging Component's parent is impossible.");
		return;
	}

	_ParentGameObject = InParent;
	if (_ParentGameObject->GetIsHierarchyInitialized())
	{
		PostConstructHierarchy();
	}
}
