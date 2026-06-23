#pragma once
#include "core/platform.h"

#if defined(OS_WINDOWS)
#include "features/os/nt/keysNT.h"
#else
#include "features/os/wayland/keysWayland.h"
#endif