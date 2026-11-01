// Integer cvars that hold a packed 32 bit colour, for settings UIs to show as a colour picker instead of a number.
//
//   REXCVAR_DEFINE_INT32(fog_color, 0, "Lighting", "");
//   const bool kFogColor = [] { rexhelpers::cvar::SetColor("fog_color"); return true; }();
//
//   if (auto format = rexhelpers::cvar::ColorOf(name)) DrawColorPicker(name, *format);

#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace rexhelpers::cvar {

// The colour's byte layout, most significant byte first, as the value reads in hex. kABGR is ImGui's ImU32.
enum class ColorFormat { kRGBA, kARGB, kABGR, kBGRA };

// Marks cvar `name` as a colour. Any thread, before or after the cvar registers.
void SetColor(std::string_view name, ColorFormat format = ColorFormat::kRGBA);
// The cvar's colour format, or nothing when it isn't a colour.
std::optional<ColorFormat> ColorOf(std::string_view name);

inline void UnpackColor(uint32_t packed, ColorFormat format, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) {
  const uint8_t b3 = uint8_t(packed >> 24), b2 = uint8_t(packed >> 16), b1 = uint8_t(packed >> 8), b0 = uint8_t(packed);
  switch (format) {
    case ColorFormat::kRGBA: r = b3, g = b2, b = b1, a = b0; break;
    case ColorFormat::kARGB: a = b3, r = b2, g = b1, b = b0; break;
    case ColorFormat::kABGR: a = b3, b = b2, g = b1, r = b0; break;
    case ColorFormat::kBGRA: b = b3, g = b2, r = b1, a = b0; break;
  }
}

inline uint32_t PackColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a, ColorFormat format) {
  uint8_t b3 = 0, b2 = 0, b1 = 0, b0 = 0;
  switch (format) {
    case ColorFormat::kRGBA: b3 = r, b2 = g, b1 = b, b0 = a; break;
    case ColorFormat::kARGB: b3 = a, b2 = r, b1 = g, b0 = b; break;
    case ColorFormat::kABGR: b3 = a, b2 = b, b1 = g, b0 = r; break;
    case ColorFormat::kBGRA: b3 = b, b2 = g, b1 = r, b0 = a; break;
  }
  return (uint32_t(b3) << 24) | (uint32_t(b2) << 16) | (uint32_t(b1) << 8) | uint32_t(b0);
}

}  // namespace rexhelpers::cvar
