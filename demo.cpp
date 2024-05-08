//
// Created by QF on 5/8/2024.
//

#include <iostream>

#include "qf_thread_pool_executor.h"

int main(int argc, const char *argv[])
{
    std::cout << "Hello, QF-ThreadPoolExecutor" << std::endl;
    QFThreadPoolExecutor p(-1);
    std::cout << "Bye,   QF-ThreadPoolExecutor" << std::endl;
    return 0;
}