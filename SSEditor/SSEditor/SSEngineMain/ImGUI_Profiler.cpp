#include "pch.h"
#include "ImGUI_Profiler.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"
#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"

void ImGUI_Profiler::PerFrame()
{
	ImGui::Begin("Profiler");
	{
		FrameOutline();
		ProfileDetail();
	}
	ImGui::End();
}

void ImGUI_Profiler::FrameOutline()
{
	if (ImGui::CollapsingHeader("Frame Outline"))
	{
		ImGui::Text("Elapsed time: %f", SSFrameInfo::GetElapsedTime());
		ImGui::Text("Delta time: %f", SSFrameInfo::GetDeltaTime());
		ImGui::Text("FPS: %f", SSFrameInfo::GetFPS());
	}
}

void ImGUI_Profiler::ProfileDetail()
{
	if (ImGui::CollapsingHeader("ProfileDetail"))
	{
		ImGui::NewLine();
		if (ImGui::Button("Capture"))
		{
			_ConsumedTickCapture = g_FrameInfoProcessor->GetDeltaTick();
			_FrequencyCapture = GetPerformanceFrequency();
			_ProfileResultCapture = g_FrameInfoProcessor->GetLastProfileResult();
		}

		ImGui::NewLine();
		ShowProfileResultCapture();


		ImGui::NewLine();
		if (ImGui::Button("Copy Capture To Clipboard"))
		{
			CopyProfilerResultToClipboard();
		}
	}
}

void ImGUI_Profiler::ShowProfileResultCapture()
{
	int32 ResultIdx = _ProfileResultCapture.GetSize() - 1;
	while (ResultIdx >= 0)
	{
		ResultIdx = ShowProfilerItem_Recursion(ResultIdx, _ConsumedTickCapture, true);
	}
}

int ImGUI_Profiler::ShowProfilerItem_Recursion(int32 ProfileResultIdx, int64 ParentConsumedTick, bool bShow)
{
	bool bIsTreeNodeOpen = false;
	ProfileResultItem Item = _ProfileResultCapture[ProfileResultIdx];
	SS::SHasherW ItemProfileName = Item.Name;
	int64 ItemConsumedTick = Item.TickEnd - Item.TickStart;


	if (bShow)
	{
		double ConsumedMS = (double)ItemConsumedTick / (double)_FrequencyCapture;
		double WholeConsumedRatio = (double)ItemConsumedTick / (double)_ConsumedTickCapture;
		double ParentRelativeConsumedRatio = (double)ItemConsumedTick / (double)ParentConsumedTick;
		WholeConsumedRatio *= 100;
		ParentRelativeConsumedRatio *= 100;

		int32 WrittenWordCnt = swprintf_s(
			_u16StrWorkTable,
			sizeof(_u16StrWorkTable) / sizeof(utf16),
			L"%ls:\t"
			L"%.3lf ms\t"
			L"%.2lf(%.2lf) %%",
			ItemProfileName.C_Str(),
			ConsumedMS,
			ParentRelativeConsumedRatio,
			WholeConsumedRatio);

		UTF16StrToUtf8Str(_u16StrWorkTable, WrittenWordCnt, _u8StrWorkTable, sizeof(_u8StrWorkTable));



		bIsTreeNodeOpen = ImGui::TreeNodeEx(_u8StrWorkTable,
			ImGuiTreeNodeFlags_SpanLabelWidth |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_Selected |
			ImGuiTreeNodeFlags_DefaultOpen);
	}

	int NextIdx = ProfileResultIdx - 1;
	while (NextIdx >= 0)
	{
		SS::SHasherW NextProfileName = _ProfileResultCapture[NextIdx].Name;
		int Result = wcsncmp(
			NextProfileName.C_Str(),
			ItemProfileName.C_Str(), ItemProfileName.GetStrLen());

		if (Result == 0)
		{
			// 자기 자신이 안보이거나, 트리가 닫혀있으면 그 순간부터 모든 자식들은 카운트만 세고 보여지면 안됨.
			NextIdx = ShowProfilerItem_Recursion(NextIdx, ItemConsumedTick, bShow && bIsTreeNodeOpen);
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

	int a = 0;
}

int ImGUI_Profiler::CopyCaptureToClipboard_Recursion(int32 ProfileResultIdx, int64 ParentConsumedTick, int32 Depth)
{
	ProfileResultItem Item = _ProfileResultCapture[ProfileResultIdx];
	SS::SHasherW ItemProfileName = Item.Name;
	int64 ItemConsumedTick = Item.TickEnd - Item.TickStart;

	double ConsumedMS = (double)ItemConsumedTick / (double)_FrequencyCapture;
	double WholeConsumedRatio = (double)ItemConsumedTick / (double)_ConsumedTickCapture;
	double ParentRelativeConsumedRatio = (double)ItemConsumedTick / (double)ParentConsumedTick;
	WholeConsumedRatio *= 100;
	ParentRelativeConsumedRatio *= 100;

	swprintf_s(
		_u16StrWorkTable,
		sizeof(_u16StrWorkTable) / sizeof(utf16),
		L"%ls:\t"
		L"%.3lf ms\t"
		L"%.2lf(%.2lf) %%",
		ItemProfileName.C_Str(),
		ConsumedMS,
		ParentRelativeConsumedRatio,
		WholeConsumedRatio);


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
