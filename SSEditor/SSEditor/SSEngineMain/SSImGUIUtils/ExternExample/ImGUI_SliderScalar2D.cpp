#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"


#include <string>
#include <cmath>
#include <algorithm>


#include "imgui_internal.h"

#include "ImGUI_SliderScalar2D.h"


using namespace ImGui;

bool SSImGUI::InputVec2(char const* pLabel, ImVec2* pValue, ImVec2 const vMinValue, ImVec2 const vMaxValue, float const fScale /*= 1.0f*/)
{
    return SliderScalar2D(pLabel, &pValue->x, &pValue->y, vMinValue.x, vMaxValue.x, vMinValue.y, vMaxValue.y, fScale);
}


bool SSImGUI::SliderScalar2D(char const* pLabel, float* fValueX, float* fValueY, const float fMinX, const float fMaxX, const float fMinY, const float fMaxY, float const fZoom /*= 1.0f*/)
{
    assert(fMinX < fMaxX);
    assert(fMinY < fMaxY);

    ImGuiID const iID = ImGui::GetID(pLabel);

    ImVec2 const vSizeSubstract = ImGui::CalcTextSize(std::to_string(1.0f).c_str()) * 1.1f;


    const float WindowContentRegionWidth = GetWindowContentRegionMax().x - GetWindowContentRegionMin().x;
    float const vSizeFull = (WindowContentRegionWidth - vSizeSubstract.x) * fZoom;
    ImVec2 const vSize(vSizeFull, vSizeFull);

    float const fHeightOffset = ImGui::GetTextLineHeight();
    ImVec2 const vHeightOffset(0.0f, fHeightOffset);

    ImVec2 vPos = GetCursorScreenPos();
    ImRect oRect(vPos + vHeightOffset, vPos + vSize + vHeightOffset);

    ImGui::Text(pLabel);

    ImGui::PushID(iID);

    ImU32 const uFrameCol = ImGui::GetColorU32(ImGuiCol_FrameBg);

    ImVec2 const vOriginPos = ImGui::GetCursorScreenPos();
    ImGui::RenderFrame(oRect.Min, oRect.Max, uFrameCol, false, 0.0f);

    float const fDeltaX = fMaxX - fMinX;
    float const fDeltaY = fMaxY - fMinY;

    bool bModified = false;
    ImVec2 const vSecurity(15.0f, 15.0f);
    if (ImGui::IsMouseHoveringRect(oRect.Min - vSecurity, oRect.Max + vSecurity) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        ImVec2 const vCursorPos = ImGui::GetMousePos() - oRect.Min;

        *fValueX = vCursorPos.x / (oRect.Max.x - oRect.Min.x) * fDeltaX + fMinX;
        *fValueY = fDeltaY - vCursorPos.y / (oRect.Max.y - oRect.Min.y) * fDeltaY + fMinY;

        bModified = true;
    }

    *fValueX = std::min(std::max(*fValueX, fMinX), fMaxX);
    *fValueY = std::min(std::max(*fValueY, fMinY), fMaxY);

    float const fScaleX = (*fValueX - fMinX) / fDeltaX;
    float const fScaleY = 1.0f - (*fValueY - fMinY) / fDeltaY;

    constexpr float fCursorOff = 10.0f;
    float const fXLimit = fCursorOff / oRect.GetWidth();
    float const fYLimit = fCursorOff / oRect.GetHeight();

    ImVec2 const vCursorPos((oRect.Max.x - oRect.Min.x) * fScaleX + oRect.Min.x, (oRect.Max.y - oRect.Min.y) * fScaleY + oRect.Min.y);

    ImDrawList* pDrawList = ImGui::GetWindowDrawList();

    ImVec4 const vBlue(70.0f / 255.0f, 102.0f / 255.0f, 230.0f / 255.0f, 1.0f); // TODO: choose from style
    ImVec4 const vOrange(255.0f / 255.0f, 128.0f / 255.0f, 64.0f / 255.0f, 1.0f); // TODO: choose from style

    ImS32 const uBlue = ImGui::GetColorU32(vBlue);
    ImS32 const uOrange = ImGui::GetColorU32(vOrange);

    constexpr float fBorderThickness = 2.0f;
    constexpr float fLineThickness = 3.0f;
    constexpr float fHandleRadius = 7.0f;
    constexpr float fHandleOffsetCoef = 2.0f;

    // Cursor
    pDrawList->AddCircleFilled(vCursorPos, 5.0f, uBlue, 16);

    // Vertical Line
    if (fScaleY > 2.0f * fYLimit)
        pDrawList->AddLine(ImVec2(vCursorPos.x, oRect.Min.y + fCursorOff), ImVec2(vCursorPos.x, vCursorPos.y - fCursorOff), uOrange, fLineThickness);
    if (fScaleY < 1.0f - 2.0f * fYLimit)
        pDrawList->AddLine(ImVec2(vCursorPos.x, oRect.Max.y - fCursorOff), ImVec2(vCursorPos.x, vCursorPos.y + fCursorOff), uOrange, fLineThickness);

    // Horizontal Line
    if (fScaleX > 2.0f * fXLimit)
        pDrawList->AddLine(ImVec2(oRect.Min.x + fCursorOff, vCursorPos.y), ImVec2(vCursorPos.x - fCursorOff, vCursorPos.y), uOrange, fLineThickness);
    if (fScaleX < 1.0f - 2.0f * fYLimit)
        pDrawList->AddLine(ImVec2(oRect.Max.x - fCursorOff, vCursorPos.y), ImVec2(vCursorPos.x + fCursorOff, vCursorPos.y), uOrange, fLineThickness);

    char pBufferX[16];
    char pBufferY[16];
    ImFormatString(pBufferX, IM_ARRAYSIZE(pBufferX), "%.5f", *(float const*)fValueX);
    ImFormatString(pBufferY, IM_ARRAYSIZE(pBufferY), "%.5f", *(float const*)fValueY);

    ImU32 const uTextCol = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);

    ImGui::SetWindowFontScale(0.75f);

    ImVec2 const vXSize = ImGui::CalcTextSize(pBufferX);
    ImVec2 const vYSize = ImGui::CalcTextSize(pBufferY);

    ImVec2 const vHandlePosX = ImVec2(vCursorPos.x, oRect.Max.y + vYSize.x * 0.5f);
    ImVec2 const vHandlePosY = ImVec2(oRect.Max.x + fHandleOffsetCoef * fCursorOff + vYSize.x, vCursorPos.y);

    if (ImGui::IsMouseHoveringRect(vHandlePosX - ImVec2(fHandleRadius, fHandleRadius) - vSecurity, vHandlePosX + ImVec2(fHandleRadius, fHandleRadius) + vSecurity) &&
        ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        ImVec2 const vCursorPos = ImGui::GetMousePos() - oRect.Min;

        *fValueX = vCursorPos.x / (oRect.Max.x - oRect.Min.x) * fDeltaX + fMinX;

        bModified = true;
    }
    else if (ImGui::IsMouseHoveringRect(vHandlePosY - ImVec2(fHandleRadius, fHandleRadius) - vSecurity, vHandlePosY + ImVec2(fHandleRadius, fHandleRadius) + vSecurity) &&
        ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        ImVec2 const vCursorPos = ImGui::GetMousePos() - oRect.Min;

        *fValueY = fDeltaY - vCursorPos.y / (oRect.Max.y - oRect.Min.y) * fDeltaY + fMinY;

        bModified = true;
    }

    pDrawList->AddText(
        ImVec2(
            std::min(std::max(vCursorPos.x - vXSize.x * 0.5f, oRect.Min.x), oRect.Min.x + vSize.x - vXSize.x),
            oRect.Max.y + fCursorOff),
        uTextCol,
        pBufferX);
    pDrawList->AddText(
        ImVec2(oRect.Max.x + fCursorOff, std::min(std::max(vCursorPos.y - vYSize.y * 0.5f, oRect.Min.y),
            oRect.Min.y + vSize.y - vYSize.y)),
        uTextCol,
        pBufferY);
    ImGui::SetWindowFontScale(1.0f);

    // Borders::Right
    pDrawList->AddCircleFilled(ImVec2(oRect.Max.x, vCursorPos.y), 2.0f, uOrange, 3);
    // Handle Right::Y
    pDrawList->AddNgonFilled(ImVec2(oRect.Max.x + fHandleOffsetCoef * fCursorOff + vYSize.x, vCursorPos.y), fHandleRadius, uBlue, 4);
    if (fScaleY > fYLimit)
        pDrawList->AddLine(ImVec2(oRect.Max.x, oRect.Min.y), ImVec2(oRect.Max.x, vCursorPos.y - fCursorOff), uBlue, fBorderThickness);
    if (fScaleY < 1.0f - fYLimit)
        pDrawList->AddLine(ImVec2(oRect.Max.x, oRect.Max.y), ImVec2(oRect.Max.x, vCursorPos.y + fCursorOff), uBlue, fBorderThickness);
    // Borders::Top
    pDrawList->AddCircleFilled(ImVec2(vCursorPos.x, oRect.Min.y), 2.0f, uOrange, 3);
    if (fScaleX > fXLimit)
        pDrawList->AddLine(ImVec2(oRect.Min.x, oRect.Min.y), ImVec2(vCursorPos.x - fCursorOff, oRect.Min.y), uBlue, fBorderThickness);
    if (fScaleX < 1.0f - fXLimit)
        pDrawList->AddLine(ImVec2(oRect.Max.x, oRect.Min.y), ImVec2(vCursorPos.x + fCursorOff, oRect.Min.y), uBlue, fBorderThickness);
    // Borders::Left
    pDrawList->AddCircleFilled(ImVec2(oRect.Min.x, vCursorPos.y), 2.0f, uOrange, 3);
    if (fScaleY > fYLimit)
        pDrawList->AddLine(ImVec2(oRect.Min.x, oRect.Min.y), ImVec2(oRect.Min.x, vCursorPos.y - fCursorOff), uBlue, fBorderThickness);
    if (fScaleY < 1.0f - fYLimit)
        pDrawList->AddLine(ImVec2(oRect.Min.x, oRect.Max.y), ImVec2(oRect.Min.x, vCursorPos.y + fCursorOff), uBlue, fBorderThickness);
    // Borders::Bottom
    pDrawList->AddCircleFilled(ImVec2(vCursorPos.x, oRect.Max.y), 2.0f, uOrange, 3);
    // Handle Bottom::X
    pDrawList->AddNgonFilled(ImVec2(vCursorPos.x, oRect.Max.y + vXSize.y * 2.0f), fHandleRadius, uBlue, 4);
    if (fScaleX > fXLimit)
        pDrawList->AddLine(ImVec2(oRect.Min.x, oRect.Max.y), ImVec2(vCursorPos.x - fCursorOff, oRect.Max.y), uBlue, fBorderThickness);
    if (fScaleX < 1.0f - fXLimit)
        pDrawList->AddLine(ImVec2(oRect.Max.x, oRect.Max.y), ImVec2(vCursorPos.x + fCursorOff, oRect.Max.y), uBlue, fBorderThickness);

    ImGui::PopID();

    ImGui::Dummy(vHeightOffset);
    ImGui::Dummy(vHeightOffset);
    ImGui::Dummy(vSize);

    char pBufferID[64];
    ImFormatString(pBufferID, IM_ARRAYSIZE(pBufferID), "Values##%d", *(ImS32 const*)&iID);

//    if (ImGui::CollapsingHeader(pBufferID))
//    {
//        float const fSpeedX = fDeltaX / 64.0f;
//        float const fSpeedY = fDeltaY / 64.0f;
//
//        char pBufferXID[64];
//        ImFormatString(pBufferXID, IM_ARRAYSIZE(pBufferXID), "X##%d", *(ImS32 const*)&iID);
//        char pBufferYID[64];
//        ImFormatString(pBufferYID, IM_ARRAYSIZE(pBufferYID), "Y##%d", *(ImS32 const*)&iID);
//
//        bModified |= ImGui::DragScalar(pBufferXID, ImGuiDataType_Float, fValueX, fSpeedX, &fMinX, &fMaxX);
//        bModified |= ImGui::DragScalar(pBufferYID, ImGuiDataType_Float, fValueY, fSpeedY, &fMinY, &fMaxY);
//    }

    return bModified;
}