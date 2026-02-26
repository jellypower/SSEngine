#include "pch.h"
#include "ImGUI_Profiler.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"
#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"
#include "SSImGUIUtils/ImGUIProfilerUtils.h"

void ImGUI_Profiler::PerFrame()
{
	ImGui::Begin("Profiler");
	{
		Show_FrameOutline();
		Show_ProfileDetail();
	}
	ImGui::End();
}

void ImGUI_Profiler::Show_FrameOutline()
{
	if (ImGui::CollapsingHeader("Frame Outline"))
	{
		ImGui::Text("Elapsed time: %f", SSFrameInfo::GetElapsedTime());
		ImGui::Text("Delta time: %f", SSFrameInfo::GetDeltaTime());
		ImGui::Text("FPS: %f", SSFrameInfo::GetFPS());
	}
}

void ImGUI_Profiler::Show_ProfileDetail()
{
	if (ImGui::CollapsingHeader("Capture Profile"))
	{
		ImGui::NewLine();
		if (ImGui::Button("Capture"))
		{
			CaptureFrame();
		}

		ImGui::NewLine();
		Show_ProfileResultCapture();


		ImGui::Separator();
		ImGui::NewLine();
		if (ImGui::Button("Copy Capture To Clipboard"))
		{
			CopyProfilerResultToClipboard();
		}

		OverFrameAutoCapture();
	}
}

void ImGUI_Profiler::Show_ProfileResultCapture()
{
	int32 ResultIdx = _ProfileResultCapture.GetSize() - 1;
	while (ResultIdx >= 0)
	{
		ResultIdx = Show_ProfilerItem_Recursion(ResultIdx, _ConsumedTickCapture, true);
	}
}

int ImGUI_Profiler::Show_ProfilerItem_Recursion(int32 ProfileResultIdx, int64 ParentConsumedTick, bool bShow)
{
	bool bIsTreeNodeOpen = false;
	const ProfileResultItem& Item = _ProfileResultCapture[ProfileResultIdx];
	SS::SHasherW ItemProfileName = Item.Name;
	int64 ItemConsumedTick = Item.TickEnd - Item.TickStart;


	if (bShow)
	{
		int32 WrittenWordCnt = ProfileResultItemToU16(
			_u16StrWorkTable,
			sizeof(_u16StrWorkTable) / sizeof(utf16),
			Item,
			_ConsumedTickCapture,
			_FrequencyCapture,
			ParentConsumedTick);
		UTF16StrToUtf8Str(_u16StrWorkTable, WrittenWordCnt, _u8StrWorkTable, sizeof(_u8StrWorkTable));


		bIsTreeNodeOpen = ImGui::TreeNodeEx(_u8StrWorkTable,
			ImGuiTreeNodeFlags_SpanLabelWidth |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_Selected);
	}

	int NextIdx = ProfileResultIdx - 1;
	while (NextIdx >= 0)
	{
		SS::SHasherW NextProfileName = _ProfileResultCapture[NextIdx].Name;
		if (NextProfileName == ItemProfileName)
		{
			break; // 같은놈이면 자식이라고 생각 안하고 다음으로 넘어가게 한다.
		}

		int Result = wcsncmp(
			NextProfileName.C_Str(),
			ItemProfileName.C_Str(), ItemProfileName.GetStrLen());

		if (Result == 0)
		{
			// bShow && bIsTreeNodeOpen -> 자기 자신이 안보이거나, 트리가 닫혀있으면 그 순간부터 모든 자식들은 카운트만 세고 보여지면 안됨.
			// 만약 NextProfileName와 네임스페이스가 겹치면 자식이니까 안으로 들어간다.
			NextIdx = Show_ProfilerItem_Recursion(NextIdx, ItemConsumedTick, bShow && bIsTreeNodeOpen);
		}
		else
		{
			break;
		}
	}


	if (bShow)
	{
		if (bIsTreeNodeOpen)
		{
			ImGui::TreePop();
		}
	}

	return NextIdx;
}

void ImGUI_Profiler::CopyProfilerResultToClipboard()
{
	int32 ResultIdx = _ProfileResultCapture.GetSize() - 1;
	_ProfileResultCaptureStr.Clear();
	while (ResultIdx >= 0)
	{
		ResultIdx = CopyCaptureToClipboard_Recursion(ResultIdx, _ConsumedTickCapture, 0);
	}

	SS_ASSERT(false); // HACK:
}

int ImGUI_Profiler::CopyCaptureToClipboard_Recursion(int32 ProfileResultIdx, int64 ParentConsumedTick, int32 Depth)
{
	ProfileResultItem Item = _ProfileResultCapture[ProfileResultIdx];
	SS::SHasherW ItemProfileName = Item.Name;
	int64 ItemConsumedTick = Item.TickEnd - Item.TickStart;


	ProfileResultItemToU16(
		_u16StrWorkTable,
		sizeof(_u16StrWorkTable) / sizeof(utf16),
		Item,
		_ConsumedTickCapture,
		_FrequencyCapture,
		ParentConsumedTick);

	for (int32 i = 0; i < Depth; i++)
	{
		_ProfileResultCaptureStr += L"\t";
	}
	_ProfileResultCaptureStr += _u16StrWorkTable;
	_ProfileResultCaptureStr += L"\n";


	int NextIdx = ProfileResultIdx - 1;
	while (NextIdx >= 0)
	{
		SS::SHasherW NextProfileName = _ProfileResultCapture[NextIdx].Name;
		if (NextProfileName == ItemProfileName)
		{
			break; // 같은놈이면 자식이라고 생각 안하고 다음으로 넘어가게 한다.
		}

		int Result = wcsncmp(
			NextProfileName.C_Str(),
			ItemProfileName.C_Str(), ItemProfileName.GetStrLen());

		if (Result == 0)
		{
			NextIdx = CopyCaptureToClipboard_Recursion(NextIdx, ItemConsumedTick, Depth + 1);
		}
		else
		{
			break;
		}
	}

	return NextIdx;
}

void ImGUI_Profiler::CaptureFrame()
{
	_ConsumedTickCapture = g_FrameInfoProcessor->GetDeltaTick();
	_FrequencyCapture = GetPerformanceFrequency();
	_ProfileResultCapture = g_FrameInfoProcessor->GetLastProfileResult();
}

void ImGUI_Profiler::OverFrameAutoCapture()
{
	ImGui::Checkbox("Auto Frame Capture", &_bOverFrameAutoCapture);

	if (_bOverFrameAutoCapture == false)
	{
		return;
	}

	double DeltaTime = g_FrameInfoProcessor->GetDeltaTime();
	if (DeltaTime < 0.016)
	{
		// 60fps 가 되면 일단 패스
		return;
	}

	_bOverFrameAutoCapture = false; // If Captured once, It disabled.
	_ConsumedTickCapture = g_FrameInfoProcessor->GetDeltaTick();
	_FrequencyCapture = GetPerformanceFrequency();
	_ProfileResultCapture = g_FrameInfoProcessor->GetLastProfileResult();
}
