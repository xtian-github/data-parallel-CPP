// Copyright (C) 2020 Intel Corporation

// SPDX-License-Identifier: MIT

#include <array>
#include <iostream>
#include <sycl/sycl.hpp>
using namespace sycl;

int main() {
  constexpr size_t size = 16;
  std::array<int, size> data;

  for (int i = 0; i < size; i++) {
    data[i] = i;
  }

  {
    buffer data_buf{data};

    queue Q;
    std::cout << "Running on device: "
              << Q.get_device().get_info<info::device::name>()
              << "\n";

    Q.submit([&](handler& h) {
#ifdef BUG
// BEGIN CODE SNIP
      accessor data_acc {data_buf, h};

      h.parallel_for(nd_range{{size}, {8}},
         [=](id<1> i) noexcept [[sycl::reqd_work_group_size(8)]] -> void {
            data_acc[i] = data_acc[i] + 1;
          });
      });
// END CODE SNIP
#else
      accessor data_acc{data_buf, h};

      h.parallel_for(
          nd_range<3>{{1, 1, size}, {1, 1, 8}},
          [=](id<3> id) noexcept [[sycl::reqd_work_group_size(1, 1, 8)]] -> void {
            int i = id[2];
            data_acc[i] = data_acc[i] + 1;
          });
      });
#endif
  }

  for (int i = 0; i < size; i++) {
    if (data[i] != i + 1) {
      std::cout << "Results did not validate at index " << i
                << "!\n";
      return -1;
    }
  }

  std::cout << "Success!\n";
  return 0;
}
