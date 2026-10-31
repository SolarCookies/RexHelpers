#include <rexhelpers/guest_caller.h>

#include <atomic>

#include <rex/logging.h>
#include <rex/runtime.h>
#include <rex/system/thread_state.h>

namespace rexhelpers {

bool GuestCallerTracked() {
  const rex::Runtime* runtime = rex::Runtime::instance();
  return runtime && !runtime->codegen_flags().skip_lr;
}

uint32_t GuestCaller() {
  if (!GuestCallerTracked()) {
    static std::atomic<bool> told{false};
    if (!told.exchange(true)) REXLOG_WARN("GuestCaller: the game was generated with skip_lr, so recompiled code never sets the link register; set skip_lr = false in the codegen config");
    return 0;
  }
  const rex::runtime::ThreadState* thread = rex::runtime::ThreadState::Get();
  const PPCContext* context = thread ? thread->context() : nullptr;
  return context ? static_cast<uint32_t>(context->lr) : 0;
}

}  // namespace rexhelpers
