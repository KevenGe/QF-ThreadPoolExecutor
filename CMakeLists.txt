cmake_minimum_required(VERSION 3.24)

project(QF_THREADPOOLEXECUTOR LANGUAGES CXX)

add_executable(demo demo.cpp
        net_thread_pool_executor.h)