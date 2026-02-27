#include "pch.h"
#include "ImGUIAssetManagerUtils.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

SS::SHasherW ImGUI_ShowAssetListCombo(EAssetType InType, const utf8* LabelName, SS::SHasherW PrevSelectedAssetName,
	ImGuiComboFlags_ ComboFlags)
{
	constexpr int32 BUFFER_SIZE = 512;

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& AssetList = AssetManager->GetAssetMap(InType);

	uint32 PrevSelectedAssetNameStrLen = PrevSelectedAssetName.GetStrLen();
	const utf16* PrevSelectedAssetNameCStr = PrevSelectedAssetName.C_Str();

	utf8 u8EquippedTexName[BUFFER_SIZE] = "EMPTY";
	UTF16StrToUtf8Str(PrevSelectedAssetNameCStr, PrevSelectedAssetNameStrLen, u8EquippedTexName, BUFFER_SIZE);


	SS::SHasherW NewlySelectedAssetName;

	if (ImGui::BeginCombo(LabelName, u8EquippedTexName, ImGuiComboFlags_WidthFitPreview))
	{
		for (const SS::pair<SS::SHasherW, IAssetBase*>& ItemPair : AssetList)
		{
			IAssetBase* AssetItem = ItemPair.second;
			SS::SHasherW AssetItemName = AssetItem->GetAssetName();
			uint32 AssetItemNameStrLen = AssetItemName.GetStrLen();
			const utf16* AssetItemNameCStr = AssetItemName.C_Str();

			utf8 u8SelectTexItemName[BUFFER_SIZE];
			UTF16StrToUtf8Str(AssetItemNameCStr, AssetItemNameStrLen, u8SelectTexItemName, BUFFER_SIZE);

			bool bIsSelectedAsset = false;
			if (PrevSelectedAssetName == AssetItemName)
			{
				bIsSelectedAsset = true;
			}

			if (ImGui::Selectable(u8SelectTexItemName, bIsSelectedAsset))
			{
				NewlySelectedAssetName = AssetItem->GetAssetName();
			}

			if (bIsSelectedAsset)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	return NewlySelectedAssetName;
}

bool ImGUI_Transform_Edit(Transform& InOutTransform)
{
	float PickedScale[3];
	PickedScale[0] = InOutTransform.Scale.X;
	PickedScale[1] = InOutTransform.Scale.Y;
	PickedScale[2] = InOutTransform.Scale.Z;

	float PickedRotation[3];
	Vector4f EulerRotation = XMEulerFromQuaternion(InOutTransform.Rotation.SimdVec);
	PickedRotation[0] = SS::RadToDegrees(EulerRotation.X);
	PickedRotation[1] = SS::RadToDegrees(EulerRotation.Y);
	PickedRotation[2] = SS::RadToDegrees(EulerRotation.Z);

	float PickedPosition[3];
	PickedPosition[0] = InOutTransform.Position.X;
	PickedPosition[1] = InOutTransform.Position.Y;
	PickedPosition[2] = InOutTransform.Position.Z;

	
	if (ImGui::InputFloat3("Scale", PickedScale))
	{
		Vector4f NewScale;
		NewScale.X = PickedScale[0];
		NewScale.Y = PickedScale[1];
		NewScale.Z = PickedScale[2];
		NewScale.W = 1;

		InOutTransform.Scale = NewScale;
		return true;
	}


	ImGui::InputFloat3("Rotation", PickedRotation, "%.0f");
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		Vector4f NewRot;

		NewRot.X = SS::DegToRadians(PickedRotation[0]);
		NewRot.Y = SS::DegToRadians(PickedRotation[1]);
		NewRot.Z = SS::DegToRadians(PickedRotation[2]);
		NewRot.W = 0;


		Quaternion NewQuatRot = Quaternion::FromEulerRotation(NewRot);
		InOutTransform.Rotation = NewQuatRot;
		return true;
	}

	if (ImGui::InputFloat3("Position", PickedPosition))
	{
		Vector4f NewPos;
		NewPos.X = PickedPosition[0];
		NewPos.Y = PickedPosition[1];
		NewPos.Z = PickedPosition[2];
		NewPos.W = 1;

		InOutTransform.Position = NewPos;
		return true;
	}

	return false;
}
