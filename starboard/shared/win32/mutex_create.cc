// Copyright 2017 The Cobalt Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#if SB_API_VERSION < 16

#include "starboard/mutex.h"

#include <windows.h>

#include "starboard/common/log.h"
#include "starboard/shared/win32/types_internal.h"

bool SbMutexCreate(SbMutex* mutex) {
  SB_COMPILE_ASSERT(sizeof(SbMutex) >= sizeof(SRWLOCK),
                    srwlock_larger_than_sb_mutex);
  if (!mutex) {
    return false;
  }
  InitializeSRWLock(SB_WIN32_INTERNAL_MUTEX(mutex));
  return true;
}

#endif  // SB_API_VERSION < 16
