// Which cvars settings UIs show: some only while another cvar has one of a few values, some only for
// developers.
//
// A game's VR settings mean nothing while VR is off, a renderer's while it isn't the renderer in use. A rule
// names the parent and the values it must have; a cvar shown nowhere still works and saves as before. Rules
// chain: a parent hidden by its own SetVisibleIf rule hides its children too. A parent kept out of lists by
// Hide doesn't: its children still follow its value.
//
// The rules are only read by settings UIs that ask IsVisible, such as the game's own cvar menu or
// RexLiveless's settings blade (Config::show_setting).

#pragma once

#include <initializer_list>
#include <string_view>

namespace rexhelpers::cvar {

// Shows cvar `name` only while cvar `parent` has one of `values`, compared without regard to case. Booleans
// are "true" and "false". A later rule for the same cvar replaces the earlier one.
void SetVisibleIf(std::string_view name, std::string_view parent, std::initializer_list<std::string_view> values);

// Never shows cvar `name` in settings UIs: a command meant for the console, say. It still works as before.
void Hide(std::string_view name);

// Leaves cvar `name` out of settings UIs unless show_all_cvars is on: for settings a player never needs, such
// as debug switches, paths and internals. It still works and saves as before.
void HideInRelease(std::string_view name);

// HideInRelease for every cvar in `category` and its subcategories ("GPU" covers "GPU/D3D12"), including ones
// registered later.
void HideCategoryInRelease(std::string_view category);

// Exempts cvar `name` from its category's HideCategoryInRelease.
void ShowInRelease(std::string_view name);

// Whether cvar `name` is hidden in release, by itself or by its category.
bool IsHiddenInRelease(std::string_view name);

// Whether a settings UI shows cvar `name`: true unless a rule (its own or a parent's) hides it, or it's hidden
// in release while show_all_cvars is off.
bool IsVisible(std::string_view name);

}  // namespace rexhelpers::cvar
