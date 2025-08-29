#include "ImGUI_GameObjectDetailViewer.h"

#include <SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h>
#include <SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightComponent.h>
#include <SSRenderer/Public/SSRendererGlobalVariableSet.h>
#include <SSRenderer/Public/RenderAsset/IAssetManager.h>
#include <SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h>
#include <SSRenderer/Public/RenderBase/IRenderer.h>

#include "SSContentsBase/Public/ContentBase/SGameObject.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "imgui.h"

void ImGUI_ShowGameObjectDetail(const SObjHashCode& GOToShow)
{
	ImGui::Begin((utf8*)u8"Object Detail");
	{
		constexpr int32 BUFFER_SIZE = 512;
		char PickedObjName[BUFFER_SIZE] = "EMPTY";
		int64 ObjectID = GOToShow.GetNativeValue();
		SGameObject* PickedInstance = dynamic_cast<SGameObject*>(GOToShow.GetSObject());

		if (PickedInstance != nullptr)
		{
			SS::SHasherW sObjectName = PickedInstance->GetObjectName();
			uint32 iObjNameLen = 0;
			const utf16* u16ObjName = sObjectName.C_Str(&iObjNameLen);
			UTF16StrToUtf8Str(u16ObjName, iObjNameLen, PickedObjName, BUFFER_SIZE);
		}

		{
			ImGui::Text((utf8*)u8"Name: %s", PickedObjName);
			ImGui::Text((utf8*)u8"ID: %lld", ObjectID);
			ImGui::Dummy(ImVec2(1, 7));
		}

		if (PickedInstance != nullptr)
		{
			ImGUI_ShowGameObjectTransform(PickedInstance);

			int32 CompCnt = PickedInstance->GetComponentCnt();
			for (int32 i=0;i<CompCnt;i++)
			{
				SComponentBase* Comp = PickedInstance->GetComponentByIdx(i);
				ImGUI_ShowComponentDetailInfo(Comp);
			}
		}
	}
	ImGui::End();
}

void ImGUI_ShowGameObjectTransform(SGameObject* PickedInstance)
{
	const Transform& transform = PickedInstance->GetTransform();


	// Set Scale
	{
		float PickedScale[3];
		PickedScale[0] = transform.Scale.X;
		PickedScale[1] = transform.Scale.Y;
		PickedScale[2] = transform.Scale.Z;
		if (ImGui::InputFloat3("Scale", PickedScale))
		{
			Vector4f NewScale;
			NewScale.X = PickedScale[0];
			NewScale.Y = PickedScale[1];
			NewScale.Z = PickedScale[2];
			NewScale.W = 1;

			PickedInstance->SetScale(NewScale);
		}
	}

	// Set Rotation
	{
		Vector4f EulerRotation = XMEulerFromQuaternion(transform.Rotation.SimdVec);

		float PickedRotation[3];
		PickedRotation[0] = SS::RadToDegrees(EulerRotation.X);
		PickedRotation[1] = SS::RadToDegrees(EulerRotation.Y);
		PickedRotation[2] = SS::RadToDegrees(EulerRotation.Z);


		ImGui::InputFloat3("Rotation", PickedRotation, "%.0f");
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Vector4f NewRot;

			NewRot.X = SS::DegToRadians(PickedRotation[0]);
			NewRot.Y = SS::DegToRadians(PickedRotation[1]);
			NewRot.Z = SS::DegToRadians(PickedRotation[2]);
			NewRot.W = 0;

			Quaternion NewQuatRot = Quaternion::FromEulerRotation(NewRot);
			PickedInstance->SetRotation(NewQuatRot);
		}
	}

	// Move Position
	{
		float PickedPosition[3];
		PickedPosition[0] = transform.Position.X;
		PickedPosition[1] = transform.Position.Y;
		PickedPosition[2] = transform.Position.Z;
		if (ImGui::InputFloat3("Position", PickedPosition))
		{
			Vector4f NewPos;
			NewPos.X = PickedPosition[0];
			NewPos.Y = PickedPosition[1];
			NewPos.Z = PickedPosition[2];
			NewPos.W = 1;

			PickedInstance->SetPosition(NewPos);
		}
	}
}


void ImGUI_ShowComponentDetailInfo(SComponentBase* ComponentToShow)
{
	if (SRenderLightComponent* RenderLight = dynamic_cast<SRenderLightComponent*>(ComponentToShow))
	{
		ImGUI_ShowLightCompDetail(RenderLight);
	}
	else if (SCubeMapRenderComponent* CubemapComp = dynamic_cast<SCubeMapRenderComponent*>(ComponentToShow))
	{
		ImGUI_ShowCubemapCompDetail(CubemapComp);
	}
}

void ImGUI_ShowLightCompDetail(SRenderLightComponent* CompToShow)
{
	const utf8* u8CompName = (utf8*)u8"RenderLightComponent";

	if (ImGui::CollapsingHeader(u8CompName))
	{
		ImGui::PushID(u8CompName);
		{
			const Vector4f LightIntensity = CompToShow->GetLightIntensity();

			float PickedPosition[4];
			PickedPosition[0] = LightIntensity.X;
			PickedPosition[1] = LightIntensity.Y;
			PickedPosition[2] = LightIntensity.Z;
			PickedPosition[3] = LightIntensity.W;

			if (ImGui::SliderFloat4("LightIntensity", PickedPosition, 0, 3))
			{
				Vector4f NewPos;
				NewPos.X = PickedPosition[0];
				NewPos.Y = PickedPosition[1];
				NewPos.Z = PickedPosition[2];
				NewPos.W = PickedPosition[3];

				CompToShow->SetLightIntensity(NewPos);
			}
		}
		ImGui::PopID();
	}
}

void ImGUI_ShowCubemapCompDetail(SCubeMapRenderComponent* CubemapToShow)
{
	const utf8* u8CompName = (utf8*)u8"CubeMapComp";
	if (ImGui::CollapsingHeader(u8CompName))
	{
		ImGui::PushID(u8CompName);
		{
			
			IAssetManager* AssetManager = g_Renderer->GetAssetManager();
			const SS::HashMap<SS::SHasherW, IAssetBase*>& TextureList = AssetManager->GetAssetMap(EAssetType::Texture);

			constexpr int32 BUFFER_SIZE = 256;


			SS::SHasherW EquippedTexName = CubemapToShow->GetCubeMapTextureAssetName();
			utf8 u8EquippedTexName[BUFFER_SIZE] = "EMPTY";
			if (EquippedTexName.IsEmpty() == false)
			{
				uint32 EquippedTexNameCStrLen = 0;
				const utf16* EquippedTexNameCStr = EquippedTexName.C_Str(&EquippedTexNameCStrLen);
				UTF16StrToUtf8Str(EquippedTexNameCStr, EquippedTexNameCStrLen, u8EquippedTexName, BUFFER_SIZE);
			}

			if (ImGui::BeginCombo("Cubemap Textures", u8EquippedTexName, ImGuiComboFlags_WidthFitPreview))
			{
				for (const SS::pair<SS::SHasherW, IAssetBase*>& TexturePairItem : TextureList)
				{
					
					ITextureAsset* TextureItem = static_cast<ITextureAsset*>(TexturePairItem.second);
					if (TextureItem->GetTextureType() != ETextureType::CubeMap)
					{
						continue;
					}

					SS::SHasherW SelectTexItemName = TextureItem->GetAssetName();
					uint32 SelectTexItemCStrLen = 0;
					const utf16* SelectTexItemCStr = SelectTexItemName.C_Str(&SelectTexItemCStrLen);

					utf8 u8SelectTexItemName[BUFFER_SIZE];
					UTF16StrToUtf8Str(SelectTexItemCStr, SelectTexItemCStrLen, u8SelectTexItemName, BUFFER_SIZE);

					bool bIsSelected = false;
					if (EquippedTexName == SelectTexItemName)
					{
						bIsSelected = true;
					}

					if (ImGui::Selectable(u8SelectTexItemName, bIsSelected))
					{
						CubemapToShow->SetCubeMapTextureAssetName(SelectTexItemName);
					}

					if (bIsSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
		ImGui::PopID();
	}
}
