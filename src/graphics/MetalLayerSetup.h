#pragma once

namespace BGFX {
// Setup Metal layer for BGFX to avoid semaphore deadlock
// Pass the Metal layer directly instead of window handle
void* cbSetupMetalLayer(void* wnd);
} // namespace BGFX
