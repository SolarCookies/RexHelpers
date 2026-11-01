// App defaults for SDK cvars: an app whose defaults differ from the SDK's (keybinds, say) sets its own before the
// config loads.
//
//   void OnConfigurePaths(rex::PathConfig& paths) override {
//     rexhelpers::cvar::SetDefault("keybind_a", "LMB");
//     ...
//   }
//
// The config file, environment and command line still win, and the config only keeps values that differ from
// the new default, so a player who never changed a setting follows the app's default.

#pragma once

#include <string_view>

namespace rexhelpers::cvar {

// Makes `value` cvar `name`'s default, and its value unless the command line or environment set it already.
// During setup only, while one thread runs: OnConfigurePaths is the last hook before the config loads. False
// when there's no such cvar or the value doesn't parse.
bool SetDefault(std::string_view name, std::string_view value);

}  // namespace rexhelpers::cvar
