// Copyright 2015 The Cobalt Authors. All Rights Reserved.
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

#include "starboard/configuration.h"

#include "build/build_config.h"
#include "starboard/client_porting/wrap_main/wrap_main.h"
#include "starboard/common/log.h"
#include "starboard/event.h"
#include "starboard/system.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {
int InitAndRunAllTests(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  SB_LOG(INFO) << "In InitAndRunAllTests";
  return RUN_ALL_TESTS();
}
}  // namespace

#if BUILDFLAG(IS_STARBOARD)
// For the Starboard OS define SbEventHandle as the entry point
SB_EXPORT STARBOARD_WRAP_SIMPLE_MAIN(InitAndRunAllTests);

#if !defined(IS_COBALT_HERMETIC_BUILD)

#if !SB_IS(EVERGREEN)
// Define main() for non-Evergreen Starboard OS.
int main(int argc, char** argv) {
  return SbRunStarboardMain(argc, argv, SbEventHandle);
}
#endif  // !SB_IS(EVERGREEN)
#else
// If the OS is not Starboard use the regular main e.g. ATV.
int main(int argc, char** argv) {
  return InitAndRunAllTests(argc, argv);
}

#endif  // !defined(IS_COBALT_HERMETIC_BUILD)
#endif  // BUILDFLAG(IS_STARBOARD)
