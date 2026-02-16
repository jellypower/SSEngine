#pragma once
#include "SObject/Public/SObjHashT.h"


class SAnimatorBaseComponent;
class SSimpleAnimatorTestComponent;
class SSkinnedMeshRenderComponent;
class SCubeMapRenderComponent;
class SRenderLightComponent;
class SComponentBase;
class SObjHashCode;
class SGameObject;
class SWorld;

class ImGUI_WorldManager : INoncopyable
{
public:
	ImGUI_WorldManager(SWorld* InWorld);

public:
	void PerFrame();

	SGameObject* GetPickedObject() const;


private:
	void ImGUI_DrawHierarchy();
	void ImGUI_DrawHierarchy_Recursion(SGameObject* Object);

private:
	void ImGUI_ShowGameObjectDetail();

	void ImGUI_ShowGameObjectTransform(SGameObject* PickedInstance);
	void ImGUI_ShowComponentDetailInfo(SComponentBase* ComponentToShow);

	void ImGUI_ShowLightCompDetail(SRenderLightComponent* CompToShow);
	void ImGUI_ShowCubemapCompDetail(SCubeMapRenderComponent* CubemapToShow);
	void ImGUI_ShowSkinnedMeshCompDetail(SSkinnedMeshRenderComponent* SkinnedMeshToShow);
	void ImGUI_ShowSimpleAnimTestComp(SSimpleAnimatorTestComponent* AnimComp);

private:
	SWorld* _BoundWorld = nullptr;


	SObjHashT<SGameObject> _PickedObject;
	SObjHashT<SGameObject> _LastPixelPickedObject;
	SObjHashT<SGameObject> _LastHieararchyPickedObject;

	int32 _PixelPickingRequestFrameCounter = 0;
};
