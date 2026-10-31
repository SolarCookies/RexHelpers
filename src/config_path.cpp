#include <rexhelpers/config_path.h>

#include <utility>

#include <rex/cvar.h>

namespace rexhelpers {
namespace {

std::filesystem::path& Storage() {
  static std::filesystem::path path;
  return path;
}

}  // namespace

void SetConfigPath(std::filesystem::path path) { Storage() = std::move(path); }

const std::filesystem::path& ConfigPath() { return Storage(); }

bool SaveConfig() {
  if (Storage().empty()) return false;
  rex::cvar::SaveConfig(Storage());
  return true;
}

}  // namespace rexhelpers
