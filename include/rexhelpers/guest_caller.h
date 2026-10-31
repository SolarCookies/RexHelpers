// Which guest code called the function a hook replaces.
//
// Recompiled code sets the link register before each call, so inside a hook it holds the return address: the
// instruction after the caller's bl. One hooked function called from many places can then act on some callers
// only:
//
//   int Damage_Hook(int actor, int amount) {
//     if (rexhelpers::GuestCaller() == 0x8251289C) amount *= 2;
//     return GuestToHostFunction<int>(__imp__Damage, actor, amount);
//   }
//
// Read it first thing in the hook. Calling back into the guest sets the link register again.
//
// It needs the game generated without skip_lr. With skip_lr = true in the codegen config, recompiled code
// never sets the link register, so there is no caller to report; GuestCaller returns 0 and the first call
// logs why.

#pragma once

#include <cstdint>

namespace rexhelpers {

// The return address of the guest call into the current hook, or 0 when there is none to report: outside a
// guest thread, or in a game generated with skip_lr.
uint32_t GuestCaller();

// Whether the game sets the link register, so GuestCaller can answer.
bool GuestCallerTracked();

}  // namespace rexhelpers
