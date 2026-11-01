#include <rexhelpers/config_path.h>

#include <array>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include <rex/cvar.h>
#include <rex/logging.h>
#include <toml++/toml.hpp>

namespace rexhelpers {
namespace {

using rex::cvar::FlagEntry;
using rex::cvar::FlagType;
using rex::cvar::Source;

std::filesystem::path& Storage() {
  static std::filesystem::path path;
  return path;
}

// Compared as numbers: an int32 cvar defined as 0xFFFFFFFF reads back as -1 but keeps "4294967295" as its
// default text.
bool IsDefault(const FlagEntry& entry, const std::string& value) {
  try {
    switch (entry.type) {
      case FlagType::Int32: return static_cast<int32_t>(std::stoll(value)) == static_cast<int32_t>(std::stoll(entry.default_value));
      case FlagType::Uint32: return static_cast<uint32_t>(std::stoll(value)) == static_cast<uint32_t>(std::stoll(entry.default_value));
      case FlagType::Int64: return std::stoll(value) == std::stoll(entry.default_value);
      case FlagType::Uint64: return std::stoull(value) == std::stoull(entry.default_value);
      case FlagType::Double: return std::stod(value) == std::stod(entry.default_value);
      default: return value == entry.default_value;
    }
  } catch (const std::exception&) {
    return value == entry.default_value;
  }
}

void Put(toml::table& table, const FlagEntry& entry, const std::string& value) {
  try {
    switch (entry.type) {
      case FlagType::Boolean: table.insert_or_assign(entry.name, value == "true"); return;
      case FlagType::Int32:
      case FlagType::Int64:
      case FlagType::Uint32: table.insert_or_assign(entry.name, static_cast<int64_t>(std::stoll(value))); return;
      // TOML integers are signed 64 bit; a larger value keeps its bit pattern, as the SDK reads it back.
      case FlagType::Uint64: table.insert_or_assign(entry.name, static_cast<int64_t>(std::stoull(value))); return;
      case FlagType::Double: table.insert_or_assign(entry.name, std::stod(value)); return;
      default: table.insert_or_assign(entry.name, value); return;
    }
  } catch (const std::exception&) {
    table.insert_or_assign(entry.name, value);
  }
}

// toml++ writes floats with 17 digits (0.1 as 0.10000000000000001), or with 6 when relaxed, which loses some.
// Floats go out as the shortest text that reads back the same; everything else through toml++, for its quoting
// and escaping. Tables (keys the SDK flattened from sections) come last, as TOML needs.
void Write(std::ostream& out, const toml::table& table) {
  toml::table sections;
  for (auto&& [key, node] : table) {
    if (node.is_table()) {
      sections.insert(key, node);
      continue;
    }
    // One key at a time, as toml++ writes it.
    std::ostringstream line;
    line << toml::table{{key, node}};
    std::string text = line.str();
    while (!text.empty() && (text.back() == '\n' || text.back() == '\r')) text.pop_back();
    if (node.is_floating_point()) {
      const double value = node.as_floating_point()->get();
      std::string number = std::isnan(value) ? "nan" : value == INFINITY ? "inf" : value == -INFINITY ? "-inf" : "";
      if (number.empty()) {
        std::array<char, 32> digits{};
        number.assign(digits.data(), std::to_chars(digits.data(), digits.data() + digits.size(), value).ptr);
        if (number.find_first_of(".e") == std::string::npos) number += ".0";
      }
      text = text.substr(0, text.rfind('=')) + "= " + number;
    }
    out << text << "\n";
  }
  if (!sections.empty()) out << "\n" << sections << "\n";
}

}  // namespace

void SetConfigPath(std::filesystem::path path) { Storage() = std::move(path); }

const std::filesystem::path& ConfigPath() { return Storage(); }

bool SaveConfig() {
  const std::filesystem::path& path = Storage();
  if (path.empty()) return false;

  // Starts from the file, so keys this run doesn't own stay: settings of another build or of a module that isn't
  // attached, and anything the SDK doesn't register.
  toml::table table;
  std::error_code error;
  if (std::filesystem::exists(path, error)) {
    try {
      table = toml::parse_file(path.string());
    } catch (const toml::parse_error& parse_error) {
      REXLOG_WARN("rexhelpers: {} doesn't parse ({}); writing it anew", path.string(), parse_error.description());
    }
  }
  for (const FlagEntry& entry : rex::cvar::GetRegistry()) {
    if (entry.type == FlagType::Command) continue;
    // A value from the command line or environment is for this run only: the file keeps its own.
    if (entry.source == Source::kCommandLine || entry.source == Source::kEnvironment) continue;
    const std::string value = entry.getter();
    if (IsDefault(entry, value)) {
      table.erase(entry.name);
    } else {
      Put(table, entry, value);
    }
  }

  std::ofstream file(path, std::ios::binary | std::ios::trunc);
  if (!file) {
    REXLOG_WARN("rexhelpers: can't write {}", path.string());
    return false;
  }
  file << "# Auto-generated cvar configuration\n";
  Write(file, table);
  return static_cast<bool>(file);
}

}  // namespace rexhelpers
