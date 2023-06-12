// Copyright (C) 2023 Intel Corporation

// SPDX-License-Identifier: MIT

#include <sycl/sycl.hpp>
using namespace sycl;

// Our example asynchronous handler function
auto handle_async_error = [](exception_list elist) {
  for (auto &e : elist) {
    try {
      std::rethrow_exception(e);
    } catch (...) {
      std::cout << "Caught SYCL ASYNC exception!!\n";
    }
  }
};

void say_device(const queue &Q) {
  std::cout << "Device : "
            << Q.get_device().get_info<info::device::name>()
            << "\n";
}

class something_went_wrong {};  // Example exception type

int main() {
  queue q{cpu_selector_v, handle_async_error};
  say_device(q);

  q.submit([&](handler &h) {
     h.host_task([]() { throw(something_went_wrong{}); });
   }).wait();

  return 0;
}

// Example output:
// Device : Intel(R) Xeon(R) Gold 6128 CPU @ 3.40GHz
// Caught SYCL ASYNC exception!!
