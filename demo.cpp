//
// Created by QF on 5/8/2024.
//

#include <iostream>

#include "qf_thread_pool_executor.h"

void slow_add(int a, int b, int & c) {
  std::cout << "use " << a << " + " << b << " = " << c << std::endl;
  c = a + b;
}

int main(int argc, const char *argv[]) {
  std::cout << "Hello, QF-ThreadPoolExecutor" << std::endl;
  // Task t;
  int a = 1;
  int b = 2;
  int c = -1;
  // t.submit(slow_add, 1, 2, std::ref(c));
  // t.execute();
  // // slow_add(1, 2, c);
  // std::cout << c << std::endl;

  QFThreadPoolExecutor qf(3);
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.submit(slow_add, 1, 2, std::ref(c));
  qf.join();
  std::cout << c << std::endl;

  std::cout << "Bye,   QF-ThreadPoolExecutor" << std::endl;
  return 0;
}