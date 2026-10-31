// Where the app's config file is, for code that saves cvars after a change.
//
// The SDK settles the path before setup and passes it to ReXApp::OnConfigurePaths, but keeps it to itself
// afterwards. The app hands it over there, once:
//
//   void OnConfigurePaths(rex::PathConfig& paths) override { rexhelpers::SetConfigPath(paths.config_path); }

#pragma once

#include <filesystem>

namespace rexhelpers {

void SetConfigPath(std::filesystem::path path);
// Empty until the app sets it.
const std::filesystem::path& ConfigPath();
// Saves every cvar to the config file. False when no path was set.
bool SaveConfig();

}  // namespace rexhelpers
