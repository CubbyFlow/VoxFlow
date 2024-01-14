// Author : snowapril

#ifndef VOXEL_FLOW_IMGUI_UTIL_HPP
#define VOXEL_FLOW_IMGUI_UTIL_HPP

struct ImGuiStyle;

namespace ImGui
{
void StyleCinder(ImGuiStyle* dst);
bool FileOpen(const char* label, const char* btn, char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns);

// PlotVar Util from https://github.com/ocornut/imgui/wiki/plot_var_example
void PlotVar(const char* label, float value, float scale_min = FLT_MAX, float scale_max = FLT_MAX, size_t buffer_size = 120);
void PlotVarFlushOldEntries();
}  // namespace ImGui

#endif