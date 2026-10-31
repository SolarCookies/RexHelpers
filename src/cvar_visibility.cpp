#include <rexhelpers/cvar_visibility.h>

#include <algorithm>
#include <cctype>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <rex/cvar.h>

// Settings UIs draw it as a checkbox of their own, so a rule below keeps it out of their lists.
REXCVAR_DEFINE_BOOL(show_all_cvars, false, "UI", "Show all Cvars");

namespace rexhelpers::cvar {
namespace {

struct Rule {
  std::string parent;               // empty: never shown
  std::vector<std::string> values;  // lower case
};

struct Rules {
  std::mutex mutex;
  std::unordered_map<std::string, Rule> visible_if = {{"show_all_cvars", Rule{}}};
  std::unordered_set<std::string> release_hidden;
  std::unordered_set<std::string> release_shown;
  std::vector<std::string> release_hidden_categories;
};

Rules& State() {
  static Rules rules;
  return rules;
}

// `category` is `parent` or one of its subcategories.
bool InCategory(std::string_view category, std::string_view parent) {
  return category.starts_with(parent) && (category.size() == parent.size() || category[parent.size()] == '/');
}

std::string Lower(std::string_view text) {
  std::string out(text);
  std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return char(std::tolower(c)); });
  return out;
}

}  // namespace

void SetVisibleIf(std::string_view name, std::string_view parent, std::initializer_list<std::string_view> values) {
  Rule rule{std::string(parent), {}};
  for (std::string_view value : values) rule.values.push_back(Lower(value));
  std::lock_guard lock(State().mutex);
  State().visible_if[std::string(name)] = std::move(rule);
}

void Hide(std::string_view name) {
  std::lock_guard lock(State().mutex);
  State().visible_if[std::string(name)] = Rule{};
}

void HideInRelease(std::string_view name) {
  std::lock_guard lock(State().mutex);
  State().release_hidden.emplace(name);
}

void HideCategoryInRelease(std::string_view category) {
  std::lock_guard lock(State().mutex);
  State().release_hidden_categories.emplace_back(category);
}

void ShowInRelease(std::string_view name) {
  std::lock_guard lock(State().mutex);
  State().release_shown.emplace(name);
}

bool IsHiddenInRelease(std::string_view name) {
  std::vector<std::string> categories;
  {
    std::lock_guard lock(State().mutex);
    const std::string key(name);
    if (State().release_shown.contains(key)) return false;
    if (State().release_hidden.contains(key)) return true;
    categories = State().release_hidden_categories;
  }
  if (categories.empty()) return false;
  // Outside the lock: the cvar registry has its own.
  const rex::cvar::FlagEntry* entry = rex::cvar::GetFlagInfo(name);
  if (!entry) return false;
  return std::any_of(categories.begin(), categories.end(), [&](const std::string& hidden) { return InCategory(entry->category, hidden); });
}

bool IsVisible(std::string_view name) {
  if (!REXCVAR_GET(show_all_cvars) && IsHiddenInRelease(name)) return false;
  std::string current(name);
  // A chain of parents, a few deep at most; a loop is cut off.
  for (int depth = 0; depth < 8; ++depth) {
    Rule rule;
    {
      std::lock_guard lock(State().mutex);
      const auto found = State().visible_if.find(current);
      if (found == State().visible_if.end()) return true;
      rule = found->second;
    }
    // Hide only keeps that one cvar out of lists; the ones that depend on its value still follow it.
    if (rule.parent.empty()) return depth > 0;
    const std::string value = Lower(rex::cvar::GetFlagByName(rule.parent));
    if (std::find(rule.values.begin(), rule.values.end(), value) == rule.values.end()) return false;
    current = rule.parent;
  }
  return true;
}

}  // namespace rexhelpers::cvar
