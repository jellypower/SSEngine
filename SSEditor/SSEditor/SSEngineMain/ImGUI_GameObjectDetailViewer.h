#pragma once

class SAnimatorBaseComponent;
class SSimpleAnimatorTestComponent;
class SSkinnedMeshRenderComponent;
class SCubeMapRenderComponent;
class SRenderLightComponent;
class SComponentBase;
class SObjHashCode;
class SGameObject;

void ImGUI_ShowGameObjectDetail(const SObjHashCode& PickedHashCode);

void ImGUI_ShowGameObjectTransform(SGameObject* PickedInstance);
void ImGUI_ShowComponentDetailInfo(SComponentBase* ComponentToShow);

void ImGUI_ShowLightCompDetail(SRenderLightComponent* CompToShow);
void ImGUI_ShowCubemapCompDetail(SCubeMapRenderComponent* CubemapToShow);
void ImGUI_ShowSkinnedMeshCompDetail(SSkinnedMeshRenderComponent* SkinnedMeshToShow);
void ImGUI_ShowSimpleAnimTestComp(SSimpleAnimatorTestComponent* AnimComp);