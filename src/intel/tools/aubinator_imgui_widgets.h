#include "imgui.h"

namespace Aubinator {

void PlotDots(const char* label, const ImVec2* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, ImVec2 scale_min = ImVec2(FLT_MAX, FLT_MAX), ImVec2 scale_max = ImVec2(FLT_MAX, FLT_MAX), ImVec2 graph_size = ImVec2(0,0), float radius = 1.0f, int stride = sizeof(ImVec2));
void PlotDots(const char* label, ImVec2 (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, ImVec2 scale_min = ImVec2(FLT_MAX, FLT_MAX), ImVec2 scale_max = ImVec2(FLT_MAX, FLT_MAX), ImVec2 graph_size = ImVec2(0,0), float radius = 1.0f);

void BeginURB(const char *label, int n_elements, float max_size, ImVec2 graph_size = ImVec2(0,0));
void URBItem(const char *label, const ImVec2& value, ImColor *out_color = NULL);
void EndURB();

};
