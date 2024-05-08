cmake_minimum_required(VERSION 3.24)

project(QF_THREADPOOLEXECUTOR LANGUAGES CXX)

add_executable(demo demo.cpp qf_thread_pool_executor.cpp)