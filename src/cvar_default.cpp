#include <rexhelpers/cvar_default.h>

#include <string>

#include <rex/cvar.h>
#include <rex/logging.h>

namespace rexhelpers::cvar {

bool SetDefault(std::string_view name, std::string_view value) {
  // The registry's lock is private to the SDK; setup runs on one thread, so the entry is ours to change.
  for (rex::cvar::FlagEntry& entry : rex::cvar::GetRegistry()) {
    if (entry.name != name) continue;
    if (entry.source == rex::cvar::Source::kDefault && !entry.setter(value)) {
      REXLOG_WARN("rexhelpers: {} can't default to '{}'", name, value);
      return false;
    }
    entry.default_value = std::string(value);
    return true;
  }
  REXLOG_WARN("rexhelpers: no cvar {} to set a default for", name);
  return false;
}

}  // namespace rexhelpers::cvar
