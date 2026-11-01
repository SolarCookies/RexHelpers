#include <rexhelpers/cvar_color.h>

#include <mutex>
#include <string>
#include <unordered_map>

namespace rexhelpers::cvar {
namespace {

struct Colors {
  std::mutex mutex;
  std::unordered_map<std::string, ColorFormat> formats;
};

Colors& State() {
  static Colors colors;
  return colors;
}

}  // namespace

void SetColor(std::string_view name, ColorFormat format) {
  std::lock_guard lock(State().mutex);
  State().formats[std::string(name)] = format;
}

std::optional<ColorFormat> ColorOf(std::string_view name) {
  std::lock_guard lock(State().mutex);
  const auto found = State().formats.find(std::string(name));
  if (found == State().formats.end()) return std::nullopt;
  return found->second;
}

}  // namespace rexhelpers::cvar
