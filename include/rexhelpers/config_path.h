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
// Saves the cvars to the config file: each one that differs from its default, with strings escaped. Keys the
// file has that no registered cvar owns stay, and so does the file's value for a cvar this run took from the
// command line or environment. False when no path was set or the file can't be written.
//
// Use it rather than rex::cvar::SaveConfig, which writes strings unescaped (a Windows path then breaks the
// file, and the next start loads defaults), drops keys it doesn't know and saves command line values.
bool SaveConfig();

}  // namespace rexhelpers
