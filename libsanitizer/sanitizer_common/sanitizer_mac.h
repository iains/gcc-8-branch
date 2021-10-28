//===-- sanitizer_mac.h -----------------------------------------*- C++ -*-===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is shared between various sanitizers' runtime libraries and
// provides definitions for OSX-specific functions.
//===----------------------------------------------------------------------===//
#ifndef SANITIZER_MAC_H
#define SANITIZER_MAC_H

#include "sanitizer_common.h"
#include "sanitizer_platform.h"

/* TARGET_OS_OSX is not present in SDKs before Darwin16 (macOS 10.12) use
   TARGET_OS_MAC (we have no support for iOS in any form for these versions,
   so there's no ambiguity).  */
#if !defined(TARGET_OS_OSX) && TARGET_OS_MAC
# define TARGET_OS_OSX 1
#endif

/* Other TARGET_OS_xxx are not present on earlier versions, define them to
   0 (we have no support for them; they are not valid targets anyway).  */
#ifndef TARGET_OS_IOS
#define TARGET_OS_IOS 0
#endif
#ifndef TARGET_OS_TV
#define TARGET_OS_TV 0
#endif
#ifndef TARGET_OS_WATCH
#define TARGET_OS_WATCH 0
#endif

#if SANITIZER_MAC
#include "sanitizer_posix.h"

namespace __sanitizer {

struct MemoryMappingLayoutData {
  int current_image;
  u32 current_magic;
  u32 current_filetype;
  ModuleArch current_arch;
  u8 current_uuid[kModuleUUIDSize];
  int current_load_cmd_count;
  char *current_load_cmd_addr;
  bool current_instrumented;
};

enum MacosVersion {
  MACOS_VERSION_UNINITIALIZED = 0,
  MACOS_VERSION_UNKNOWN,
  MACOS_VERSION_LEOPARD,
  MACOS_VERSION_SNOW_LEOPARD,
  MACOS_VERSION_LION,
  MACOS_VERSION_MOUNTAIN_LION,
  MACOS_VERSION_MAVERICKS,
  MACOS_VERSION_YOSEMITE,
  MACOS_VERSION_UNKNOWN_NEWER
};

MacosVersion GetMacosVersion();

char **GetEnviron();

void RestrictMemoryToMaxAddress(uptr max_address);

}  // namespace __sanitizer

extern "C" {
static char __crashreporter_info_buff__[__sanitizer::kErrorMessageBufferSize] =
  {};
static const char *__crashreporter_info__ __attribute__((__used__)) =
  &__crashreporter_info_buff__[0];
asm(".desc ___crashreporter_info__, 0x10");
} // extern "C"

namespace __sanitizer {
static BlockingMutex crashreporter_info_mutex(LINKER_INITIALIZED);

INLINE void CRAppendCrashLogMessage(const char *msg) {
  BlockingMutexLock l(&crashreporter_info_mutex);
  internal_strlcat(__crashreporter_info_buff__, msg,
                   sizeof(__crashreporter_info_buff__)); }
}  // namespace __sanitizer

#endif  // SANITIZER_MAC
#endif  // SANITIZER_MAC_H
