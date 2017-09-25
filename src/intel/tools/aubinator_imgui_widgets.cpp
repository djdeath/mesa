#include "aubinator_imgui_widgets.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_internal.h"

using namespace ImGui;

namespace Aubinator {

/* Sampler positions */

void PlotDotsEx(const char* label, ImVec2 (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, ImVec2 scale_min, ImVec2 scale_max, ImVec2 graph_size, float radius)
{
   ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    if (graph_size.x == 0.0f)
        graph_size.x = CalcItemWidth();
    if (graph_size.y == 0.0f)
        graph_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, NULL))
        return;
    const bool hovered = ItemHoverable(inner_bb, 0);

    // Determine scale from values if not specified
    if (scale_min.x == FLT_MAX || scale_min.y == FLT_MAX ||
        scale_max.x == FLT_MAX || scale_max.y == FLT_MAX)
    {
        ImVec2 v_min(FLT_MAX, FLT_MAX);
        ImVec2 v_max(-FLT_MAX, -FLT_MAX);
        for (int i = 0; i < values_count; i++)
        {
            const ImVec2 v = values_getter(data, i);
            v_min = ImVec2(ImMin(v_min.x, v.x), ImMin(v_min.y, v.y));
            v_max = ImVec2(ImMax(v_max.x, v.x), ImMax(v_max.y, v.y));
        }
        if (scale_min.x == FLT_MAX)
            scale_min.x = v_min.x;
        if (scale_min.y == FLT_MAX)
            scale_min.y = v_min.y;
        if (scale_max.x == FLT_MAX)
            scale_max.x = v_max.x;
        if (scale_max.y == FLT_MAX)
            scale_max.y = v_max.y;
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);


    ImVec2 max_deltas(scale_max.x - scale_min.x, scale_max.y - scale_min.y);
    ImVec2 paint_size = inner_bb.GetSize();
    for (int n = 0; n < values_count; n++)
    {
       const ImVec2 v = values_getter(data, n + values_offset);
       const ImVec2 point(inner_bb.GetTL() +
                      ImVec2(paint_size.x * (v.x - scale_min.x) / max_deltas.x,
                             paint_size.y * (v.y - scale_min.y) / max_deltas.y));

       ImRect square(point, point);
       square.Expand(radius);
       if (hovered && square.Contains(g.IO.MousePos))
           SetTooltip("%fx%f", v.x, v.y);

       window->DrawList->AddCircleFilled(point, radius,
                                         GetColorU32(ImGuiCol_PlotLines), 12);
    }

    // Text overlay
    if (overlay_text)
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f,0.0f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}

struct ImGuiPlotDotsArrayGetterData
{
    const ImVec2* Values;
    int Stride;

    ImGuiPlotDotsArrayGetterData(const ImVec2* values, int stride) { Values = values; Stride = stride; }
};

static ImVec2 PlotDots_ArrayGetter(void* data, int idx)
{
    ImGuiPlotDotsArrayGetterData* plot_data = (ImGuiPlotDotsArrayGetterData*)data;
    const ImVec2 v = *(ImVec2*)(void*)((unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
    return v;
}

void PlotDots(const char* label, const ImVec2* values, int values_count, int values_offset, const char* overlay_text, ImVec2 scale_min, ImVec2 scale_max, ImVec2 graph_size, float radius, int stride)
{
    ImGuiPlotDotsArrayGetterData data(values, stride);
    PlotDotsEx(label, &PlotDots_ArrayGetter, (void*)&data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, radius);
}

void PlotDots(const char* label, ImVec2 (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, ImVec2 scale_min, ImVec2 scale_max, ImVec2 graph_size, float radius)
{
    PlotDotsEx(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, radius);
}

/* URB */

static bool urb_skip = true;
static int urb_element = 0, urb_n_elements = 1;
static float urb_size = 1;
static ImRect urb_inner_bb;

void BeginURB(const char *label, int n_elements, float max_size, ImVec2 graph_size)
{
   ImGuiWindow* window = GetCurrentWindow();
   if (window->SkipItems)
      return;

   urb_skip = false;

   ImGuiContext& g = *GImGui;
   const ImGuiStyle& style = g.Style;

   const ImVec2 label_size = CalcTextSize(label, NULL, true);
   if (graph_size.x == 0.0f)
      graph_size.x = CalcItemWidth();
   if (graph_size.y == 0.0f)
      graph_size.y = label_size.y + (style.FramePadding.y * 2);

   const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
   const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
   const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
   ItemSize(total_bb, style.FramePadding.y);
   if (!ItemAdd(total_bb, NULL))
      return;

   RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

   urb_n_elements = n_elements;
   urb_element = 0;
   urb_inner_bb = inner_bb;
   urb_size = max_size;
}

void URBItem(const char *label, const ImVec2& value, ImColor *out_color)
{
   if (urb_skip)
      return;

   const ImRect rect(urb_inner_bb.GetTL() + ImVec2((value.x / urb_size) * urb_inner_bb.GetWidth(), 0),
                     urb_inner_bb.GetBL() + ImVec2(((value.x + value.y) / urb_size) * urb_inner_bb.GetWidth(), 0));

   const bool hovered = ItemHoverable(rect, 0);
   float r, g, b;
   ImGui::ColorConvertHSVtoRGB(urb_element * 1.0f / urb_n_elements,
                               hovered ? 0.5f : 1.0f, 1.0f, r, g, b);
   if (hovered)
      SetTooltip("%s: offset=%.0f size=%.0f ", label, value.x, value.y);

   ImGuiWindow* window = GetCurrentWindow();
   window->DrawList->AddRectFilled(rect.GetTL(), rect.GetBR(), ImColor(r, g, b));

   if (out_color) {
      ImGui::ColorConvertHSVtoRGB(urb_element * 1.0f / urb_n_elements, 1.0f, 1.0f, r, g, b);
      *out_color = ImColor(r, g, b);
   }

   urb_element++;
}

void EndURB()
{
   urb_skip = true;
}

}
