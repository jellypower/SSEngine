#pragma once


class SRenderLightComponent;
class SComponentBase;
class SObjHashCode;
class SGameObject;

void ImGUI_ShowGameObjectDetail(const SObjHashCode& GOToShow);

void ImGUI_ShowGameObjectTransform(SGameObject* PickedInstance);
void ImGUI_ShowComponentDetailInfo(SComponentBase* ComponentToShow);

void ImGUI_ShowLightCompDetail(SRenderLightComponent* CompToShow);