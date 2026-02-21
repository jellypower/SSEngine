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
	void ImGUI_Hierarchy();
	void ImGUI_Hierarchy_Recursion(SGameObject* Object);

private:
	void ImGUI_GODetail();
	void ImGUI_GODetail_Transform(SGameObject* PickedInstance);
	void ImGUI_GODetail_CompItem(SComponentBase* ComponentToShow);
	void ImGUI_GODetail_CompItem_LightComp(SRenderLightComponent* CompToShow);
	void ImGUI_GODetail_CompItem_CubemapComp(SCubeMapRenderComponent* CubemapToShow);
	void ImGUI_GODetail_CompItem_SkinnedMeshComp(SSkinnedMeshRenderComponent* SkinnedMeshToShow);
	void ImGUI_GODetail_CompItem_SimpleAnimTestComp(SSimpleAnimatorTestComponent* AnimComp);

private:
	void ImGUI_Spawner();


private:
	SWorld* _BoundWorld = nullptr;


	SObjHashT<SGameObject> _PickedObject;
	SObjHashT<SGameObject> _LastPixelPickedObject;
	SObjHashT<SGameObject> _LastHieararchyPickedObject;

	int32 _PixelPickingRequestFrameCounter = 0;

private:
	SS::SHasherW _SpawnerSelectedMdlc;
	Transform _SpawnerTransform;
};
