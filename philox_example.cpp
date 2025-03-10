#include <stdint.h> 

// Salmon et al. SC 2011. Parallel random numbers: as easy as 1, 2, 3.
//   http://www.thesalmons.org/john/random123/papers/random123sc11.pdf

/* 
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================
*/

uint32_t kPhiloxW32A = 0x9E3779B9;
uint32_t kPhiloxW32B = 0xBB67AE85;
uint32_t kPhiloxM4x32A = 0xD2511F53;
uint32_t kPhiloxM4x32B = 0xCD9E8D57;

uint32_t counter_[4];
uint32_t key_[2];

int philox_initialized = false;
void philox_reset() {
    counter_[0] = 1;
    counter_[1] = 2;
    counter_[2] = 3;
    counter_[3] = 3;
    key_[0] = 56;
    key_[1] = 712;
}
// Skip the specified number of samples of 128-bits in the current stream.
void philox_skip(uint64_t count) {
  const uint32_t count_lo = static_cast<uint32_t>(count);
  uint32_t count_hi = static_cast<uint32_t>(count >> 32);

  counter_[0] += count_lo;
  if (counter_[0] < count_lo) {
    ++count_hi;
  }

  counter_[1] += count_hi;
  if (counter_[1] < count_hi) {
    if (++counter_[2] == 0) {
      ++counter_[3];
    }
  }
}

// Helper function to return the lower and higher 32-bits from two 32-bit
// integer multiplications.
static void multiply_high_low(uint32_t a, uint32_t b, uint32_t* result_low,
                              uint32_t* result_high) {
  uint64_t product = static_cast<uint64_t>(a) * b;
  *result_low = static_cast<uint32_t>(product);
  *result_high = static_cast<uint32_t>(product >> 32);
}

// Helper function for a single round of the underlying Philox algorithm.
void philox_oneround(uint32_t counter[4], uint32_t key[2]) {
  uint32_t lo0;
  uint32_t hi0;
  multiply_high_low(kPhiloxM4x32A, counter[0], &lo0, &hi0);

  uint32_t lo1;
  uint32_t hi1;
  multiply_high_low(kPhiloxM4x32B, counter[2], &lo1, &hi1);

  uint32_t result[4];
  result[0] = hi1 ^ counter[1] ^ key[0];
  result[1] = lo1;
  result[2] = hi0 ^ counter[3] ^ key[1];
  result[3] = lo0;
  counter[0] = result[0];
  counter[1] = result[1];
  counter[2] = result[2];
  counter[3] = result[3];
}

void philox_raisekey(uint32_t key[2]) {
  key[0] += kPhiloxW32A;
  key[1] += kPhiloxW32B;
}

// Returns a group of four random numbers using the underlying Philox
// algorithm.
void philox_next4(uint32_t out[4]) {
  for (int i=0; i<4; i++)
    out[i] = counter_[i];
  uint32_t key[2] = {key_[0], key_[1]};
  for (int i=0; i<10; i++) {
    philox_oneround(out, key);
    if (i == 9)
      philox_skip(1);
    else
      philox_raisekey(key);
  }
}

uint64_t myrand() {
  if (!philox_initialized) {
    philox_initialized = true;
    philox_reset();
  }
  uint32_t out[4];
  philox_next4(out);
  return ((uint64_t*)&out)[0];
}

/// The above implements a counter-based random number algorithm
/// Specifically it enables you to skip forward in the future by modifying
/// a given counter variable. 
/// For example, consider the following code: https://godbolt.org/z/roEY46GeG
#include <stdio.h>
int main(int argc, char *argv[]) {
  try {
    // Default datasets
    std::string regression_dataset = "benchmark/diabetes.csv";
    std::string classification_dataset = "benchmark/cancer.csv";
    std::string housing_dataset = "benchmark/housing.csv";

    // Check if argument is provided
    if (argc < 2) {
      std::cerr << "Error: Missing dataset argument" << std::endl;
      return 1;
    }

    std::string arg_dset(argv[1]);

    if (arg_dset == "diabetes") {
      run_symbolic_regression(regression_dataset);
      return 0;
    } 
    
    if (arg_dset == "cancer") {
      run_symbolic_classification(classification_dataset);
      return 0;
    } 
    
    if (arg_dset == "housing") {
      run_symbolic_regression(housing_dataset);
      return 0;
    }

    std::cerr << "Error: Unknown dataset '" << arg_dset << "'" << std::endl;
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

