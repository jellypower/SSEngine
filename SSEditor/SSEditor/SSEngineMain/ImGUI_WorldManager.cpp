#include "pch.h"
#include "ImGUI_WorldManager.h"

#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"


#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSEngineDefault/Public/RawInput/SSInput.h"
#include "SSGAL/Public/SSGALInlineSettings.h"


#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SSkinnedMeshRenderComponent.h"
#include "SSContentsBase/Public/AnimComponents/SSimpleAnimatorTestComponent.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSImGUIUtils/ImGUIAssetManagerUtils.h"


ImGUI_WorldManager::ImGUI_WorldManager(SWorld* InWorld)
{
	_BoundWorld = InWorld;
}

void ImGUI_WorldManager::PerFrame()
{
	SCOPE_PROFILE(ImGUI_WorldManager);

	if (SSInput::GetMouseDown(EMouseCode::MOUSE_LEFT))
	{
		Vector2i32 MousePos = SSInput::GetMousePos();
		g_Renderer->RequestPixelPicking(MousePos.X, MousePos.Y);
		_PixelPickingRequestFrameCounter = GAL_NESTED_FRAME_CNT + 1; // PixelPicking용 프레임버퍼가 2프레임 뒤에 그려져서 그걸 생각해야함.
	}

	if (_PixelPickingRequestFrameCounter >= 0)
	{
		_PixelPickingRequestFrameCounter--;
	}

	if (_PixelPickingRequestFrameCounter == 0)
	{
		SObjHashCode PixelPickedObjID = g_Renderer->GetPixelPickedObjectID();
		SObjectBase* SObj = PixelPickedObjID.GetSObject();

		if (SComponentBase* PickedComponent = dynamic_cast<SComponentBase*>(SObj))
		{
			_LastPixelPickedObject = PickedComponent->GetGameObject();
		}
		else if (SGameObject* CastedPickedGameObject = dynamic_cast<SGameObject*>(SObj))
		{
			_LastPixelPickedObject = CastedPickedGameObject;
		}
	}


	if (_LastPixelPickedObject != nullptr)
	{
		_PickedObject = _LastPixelPickedObject;
	}

	if (_LastHieararchyPickedObject != nullptr) // 우선순위 더 높음
	{
		_PickedObject = _LastHieararchyPickedObject;
	}

	_LastPixelPickedObject = nullptr;
	_LastHieararchyPickedObject = nullptr;

	ImGUI_Hierarchy();
	ImGUI_GODetail();
	ImGUI_Spawner();
}

SGameObject* ImGUI_WorldManager::GetPickedObject() const
{
	return _PickedObject.Get();
}

void ImGUI_WorldManager::ImGUI_Hierarchy()
{
	if (ImGui::Begin("Hieararchy"))
	{
		if (ImGui::BeginChild("SceneTree", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			SGameObject* RootObject = _BoundWorld->GetWorldRootObject();

			int32 ChildCnt = RootObject->GetChildCnt();

			for (int i = 0; i < ChildCnt; i++)
			{
				ImGUI_Hierarchy_Recursion(RootObject->GetChild(i));
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void ImGUI_WorldManager::ImGUI_Hierarchy_Recursion(SGameObject* Object)
{
	int32 ChildCnt = Object->GetChildCnt();

	SS::SHasherW sObjectName = Object->GetObjectName();
	uint32 iObjectNameLen = sObjectName.GetStrLen();
	const utf16* u16ObjectName = sObjectName.C_Str();

	utf8 u8ObjectName[SHASHER_STRLEN_MAX];
	UTF16StrToUtf8Str(u16ObjectName, iObjectNameLen, u8ObjectName, sizeof(u8ObjectName));

	SS::StringW HashCodeStr = ToStringW(Object->GetHashCode());
	const utf16* u16HashCodeStr = HashCodeStr.C_Str();
	const int32 HashCodeStrLen = HashCodeStr.GetStrLen();
	utf8 u8ObjectID[SHASHER_STRLEN_MAX];
	UTF16StrToUtf8Str(u16HashCodeStr, HashCodeStrLen, u8ObjectID, sizeof(u8ObjectID));

	ImGui::PushID(u8ObjectID);
	if (ImGui::TreeNodeEx(u8ObjectName,
		ImGuiTreeNodeFlags_SpanLabelWidth |
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_Selected |
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::IsItemClicked(0))
		{
			_LastHieararchyPickedObject = Object;
		}

		for (int i = 0; i < ChildCnt; i++)
		{
			ImGUI_Hierarchy_Recursion(Object->GetChild(i));
		}

		ImGui::TreePop();
	}
	ImGui::PopID();
}

void ImGUI_WorldManager::ImGUI_GODetail()
{
	ImGui::Begin((utf8*)u8"Object Detail");
	{
		constexpr int32 BUFFER_SIZE = 512;
		char PickedObjName[BUFFER_SIZE] = "EMPTY";
		int64 ObjectID = _PickedObject.GetHashCode().GetNativeValue();
		SObjectBase* PickedObject = _PickedObject.Get();


		SGameObject* PickedGameObject = nullptr;
		if (SComponentBase* PickedComponent = dynamic_cast<SComponentBase*>(PickedObject))
		{
			PickedGameObject = PickedComponent->GetGameObject();
		}
		else if (SGameObject* CastedPickedGameObject = dynamic_cast<SGameObject*>(PickedObject))
		{
			PickedGameObject = CastedPickedGameObject;
		}

		if (PickedGameObject != nullptr)
		{
			SS::SHasherW sObjectName = PickedGameObject->GetObjectName();
			uint32 iObjNameLen = sObjectName.GetStrLen();
			const utf16* u16ObjName = sObjectName.C_Str();
			UTF16StrToUtf8Str(u16ObjName, iObjNameLen, PickedObjName, BUFFER_SIZE);
		}

		{
			ImGui::Text((utf8*)u8"Name: %s", PickedObjName);
			ImGui::Text((utf8*)u8"ID: %lld", ObjectID);
			ImGui::Dummy(ImVec2(1, 7));
		}

		if (PickedGameObject != nullptr)
		{
			ImGUI_GODetail_Transform(PickedGameObject);

			int32 CompCnt = PickedGameObject->GetComponentCnt();
			for (int32 i=0;i<CompCnt;i++)
			{
				SComponentBase* Comp = PickedGameObject->GetComponentByIdx(i);
				ImGUI_GODetail_CompItem(Comp);
			}
		}
	}
	ImGui::End();
}

void ImGUI_WorldManager::ImGUI_GODetail_Transform(SGameObject* PickedInstance)
{
	Transform CurTransform = PickedInstance->GetTransform();

	if (ImGUI_Transform_Edit(CurTransform))
	{
		PickedInstance->SetTransform(CurTransform);
	}
}


void ImGUI_WorldManager::ImGUI_GODetail_CompItem(SComponentBase* ComponentToShow)
{
	if (SRenderLightComponent* RenderLight = dynamic_cast<SRenderLightComponent*>(ComponentToShow))
	{
		ImGUI_GODetail_CompItem_LightComp(RenderLight);
	}
	else if (SCubeMapRenderComponent* CubemapComp = dynamic_cast<SCubeMapRenderComponent*>(ComponentToShow))
	{
		ImGUI_GODetail_CompItem_CubemapComp(CubemapComp);
	}
	else if (SSkinnedMeshRenderComponent* SkinnedMeshComp = dynamic_cast<SSkinnedMeshRenderComponent*>(ComponentToShow))
	{
		ImGUI_GODetail_CompItem_SkinnedMeshComp(SkinnedMeshComp);
	}
	else if (SSimpleAnimatorTestComponent* AnimatorComp = dynamic_cast<SSimpleAnimatorTestComponent*>(ComponentToShow))
	{
		ImGUI_GODetail_CompItem_SimpleAnimTestComp(AnimatorComp);
	}
}

void ImGUI_WorldManager::ImGUI_GODetail_CompItem_LightComp(SRenderLightComponent* CompToShow)
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

void ImGUI_WorldManager::ImGUI_GODetail_CompItem_CubemapComp(SCubeMapRenderComponent* CubemapToShow)
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
				uint32 EquippedTexNameCStrLen = EquippedTexName.GetStrLen();
				const utf16* EquippedTexNameCStr = EquippedTexName.C_Str();
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
					uint32 SelectTexItemCStrLen = SelectTexItemName.GetStrLen();
					const utf16* SelectTexItemCStr = SelectTexItemName.C_Str();

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

void ImGUI_WorldManager::ImGUI_GODetail_CompItem_SkinnedMeshComp(SSkinnedMeshRenderComponent* SkinnedMeshToShow)
{
	const utf8* u8CompName = (utf8*)u8"SSkinnedMeshRenderComponent";
	if (ImGui::CollapsingHeader(u8CompName))
	{
		ImGui::PushID(u8CompName);
		{
			constexpr int32 BUFFER_SIZE = 256;


			SS::SHasherW MeshName = SkinnedMeshToShow->GetMeshAssetName();
			utf8 u8ModelName[BUFFER_SIZE] = "EMPTY";
			if (MeshName.IsEmpty() == false)
			{
				uint32 EquippedTexNameCStrLen = MeshName.GetStrLen();
				const utf16* EquippedTexNameCStr = MeshName.C_Str();
				UTF16StrToUtf8Str(EquippedTexNameCStr, EquippedTexNameCStrLen, u8ModelName, BUFFER_SIZE);
			}

			ImGui::Text("Model Name: %s", u8ModelName);


			if (ImGui::Button("ApplyModelAssetChange"))
			{
				SkinnedMeshToShow->ApplyModelAssetChange();
			}

		}
		ImGui::PopID();
	}
}

void ImGUI_WorldManager::ImGUI_GODetail_CompItem_SimpleAnimTestComp(SSimpleAnimatorTestComponent* AnimComp)
{
	const utf8* u8CompName = (utf8*)u8"SSimpleAnimatorTestComponent";
	if (ImGui::CollapsingHeader(u8CompName))
	{
		ImGui::PushID(u8CompName);
		{

			IAssetManager* AssetManager = g_Renderer->GetAssetManager();
			const SS::HashMap<SS::SHasherW, IAssetBase*>& RenderAnimList = AssetManager->GetAssetMap(EAssetType::RenderAnim);

			constexpr int32 BUFFER_SIZE = 256;


			SS::SHasherW EquippedAnimAssetName = AnimComp->GetRenderAnimAssetName();
			utf8 u8EquippedAnimName[BUFFER_SIZE] = "EMPTY";
			if (EquippedAnimAssetName.IsEmpty() == false)
			{
				uint32 EquippedAnimNameCStrLen = EquippedAnimAssetName.GetStrLen();
				const utf16* EquippedAnimNameCStr = EquippedAnimAssetName.C_Str();
				UTF16StrToUtf8Str(EquippedAnimNameCStr, EquippedAnimNameCStrLen, u8EquippedAnimName, BUFFER_SIZE);
			}

			if (ImGui::BeginCombo("Anim Assets", u8EquippedAnimName, ImGuiComboFlags_WidthFitPreview))
			{
				for (const SS::pair<SS::SHasherW, IAssetBase*>& AnimPairItem : RenderAnimList)
				{
					IRenderAnimAsset* RenderAnimItem = static_cast<IRenderAnimAsset*>(AnimPairItem.second);
					
					SS::SHasherW SelectAnimItemName = RenderAnimItem->GetAssetName();
					uint32 SelectAnimItemCStrLen = SelectAnimItemName.GetStrLen();
					const utf16* SelectAnimItemCStr = SelectAnimItemName.C_Str();

					utf8 u8SelectTexItemName[BUFFER_SIZE];
					UTF16StrToUtf8Str(SelectAnimItemCStr, SelectAnimItemCStrLen, u8SelectTexItemName, BUFFER_SIZE);

					bool bIsSelected = false;
					if (EquippedAnimAssetName == SelectAnimItemName)
					{
						bIsSelected = true;
					}

					if (ImGui::Selectable(u8SelectTexItemName, bIsSelected))
					{
						AnimComp->SetRenderAnimAsset(SelectAnimItemName);
					}

					if (bIsSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}


			if (AnimComp->GetRenderAnimAssetName().IsEmpty() == false)
			{
				bool bIsPlaying = AnimComp->IsOnPause() == false;
				const char* BtnLabel = bIsPlaying ? "Playing" : "Play";
				if (ImGui::Button(BtnLabel))
				{
					bool bPause = bIsPlaying;
					AnimComp->SetPauseAnim(bPause);
				}

				float Duration = AnimComp->GetAnimDuration();
				float FrameTime = AnimComp->GetWholeFrameTime();
				FrameTime = fmodf(FrameTime, Duration);
				if (ImGui::SliderFloat("float", &FrameTime, 0.0f, Duration))
				{
					AnimComp->SetPauseAnim(true);
					AnimComp->SetWholeFrameTime(FrameTime);
				}
			}

			{
				bool bDrawDebug = AnimComp->GetDrawDebugResultPose();
				ImGui::Checkbox("Draw Debug Pose", &bDrawDebug);
				AnimComp->SetDrawDebugResultPose(bDrawDebug);
			}

		}
		ImGui::PopID();
	}
}

void ImGUI_WorldManager::ImGUI_Spawner()
{
	if (ImGui::Begin("Spawner"))
	{
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "SpawnTransform");
		ImGUI_Transform_Edit(_SpawnerTransform);

		{
			ImGui::Separator();
			ImGui::NewLine();

			SS::SHasherW NewlySelectMdlc = ImGUI_ShowAssetListCombo(EAssetType::ModelCombination, "Selected Mdlc", _SpawnerSelectedMdlc);
			if (NewlySelectMdlc.IsEmpty() == false)
			{
				_SpawnerSelectedMdlc = NewlySelectMdlc;
			}

			if (ImGui::Button("Spawn Mdlc"))
			{
				if (_SpawnerSelectedMdlc.IsEmpty() == false)
				{
					SGameObject* NewMdlc = SRendererUtil::InstantiateModelObjTree(_SpawnerSelectedMdlc);
					if (NewMdlc == nullptr)
					{
						return;
					}

					NewMdlc->CreateComponent<SSimpleAnimatorTestComponent>(L"AnimatorComp");
					NewMdlc->SetTransform(_SpawnerTransform);
					_BoundWorld->AddToWorld(NewMdlc);
				}
			}
		}

		{
			ImGui::Separator();
			ImGui::NewLine();

			SS::SHasherW NewlySelectMesh = ImGUI_ShowAssetListCombo(EAssetType::Mesh, "Selected Mesh", _SpawnerSelectedMesh);
			if (NewlySelectMesh.IsEmpty() == false)
			{
				_SpawnerSelectedMesh = NewlySelectMesh;
			}

			if (ImGui::Button("Spawn Mesh"))
			{
				if (_SpawnerSelectedMesh.IsEmpty() == false)
				{
					SGameObject* NewMesh = SRendererUtil::InstantiateMesh(_SpawnerSelectedMesh);
					if (NewMesh == nullptr)
					{
						return;
					}

					NewMesh->SetTransform(_SpawnerTransform);
					_BoundWorld->AddToWorld(NewMesh);
				}
			}
		}

	}
	ImGui::End();
}
