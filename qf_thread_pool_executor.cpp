//
// Created by QF on 5/8/2024.
//

#include "qf_thread_pool_executor.h"

#include <iostream>

QFThreadPoolExecutor::QFThreadPoolExecutor(const int thread_num)
{
    std::cout << "Hello, I'm qf_thread_pool_executor" << std::endl;
    std::cout << "thread num = " << thread_num << std::endl;
}