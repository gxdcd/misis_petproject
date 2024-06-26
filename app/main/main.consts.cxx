module;

#pragma region Header
#include <imgui_impl.hxx>
module main:consts;
#pragma endregion

#pragma region Constants

#ifndef NDEBUG
const auto WINDOW_TITLE = "Find-Replace (Debug)";
#else
const auto WINDOW_TITLE = "Find-Replace";
#endif

const auto REFRESH_RATE_FPS = 25.0f;

const auto FULL_SIZE_WINDOW_FLAGS
    // Never load/save settings in .ini file
    = ImGuiWindowFlags_NoSavedSettings
      // Disable title-bar
      // Disable user resizing with the lower-right grip
      // Disable user moving the window
      // Disable user collapsing window by double-clicking on it.
      | ImGuiWindowFlags_NoDecoration;

#pragma endregion