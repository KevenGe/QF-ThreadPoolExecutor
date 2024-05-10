//
// Created by QF on 5/8/2024.
//

#ifndef QF_THREAD_POOL_EXECUTOR_H
#define QF_THREAD_POOL_EXECUTOR_H

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

// ------------------------------------------------------------------------- //

class Task {
 public:
  template <typename Func, typename... Args>
  void submit(Func &&func, Args &&...args);
  void execute();

 private:
  std::function<void()> func;
};

template <typename Func, typename... Args>
void Task::submit(Func &&func, Args &&...args) {
  auto t = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
  this->func = [t]() { t(); };
}

inline void Task::execute() { this->func(); }

// ------------------------------------------------------------------------- //

class TaskQueue {
 public:
  TaskQueue();
  ~TaskQueue();

  bool empty();

  int size();

  void push_task(const Task &task);

  Task pop_task();

 private:
  std::mutex op_mutex;
  std::queue<Task> que;
};

inline TaskQueue::TaskQueue() {}

inline TaskQueue::~TaskQueue() {}

inline bool TaskQueue::empty() {
  std::unique_lock<std::mutex>(op_mutex);
  return que.empty();
}

inline int TaskQueue::size() {
  std::unique_lock<std::mutex>(op_mutex);
  return que.size();
}

inline void TaskQueue::push_task(const Task &task) {
  std::unique_lock<std::mutex>(op_mutex);
  que.emplace(task);
}

inline Task TaskQueue::pop_task() {
  std::unique_lock<std::mutex>(op_mutex);
  Task task = que.front();
  que.pop();
  return std::move(task);
}

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

class QFThreadPoolExecutor {
 public:
  QFThreadPoolExecutor() = delete;

  explicit QFThreadPoolExecutor(int thread_num);

  ~QFThreadPoolExecutor();

  void map();

  template <typename Func, typename... Args>
  void submit(Func &&func, Args &&...args);

  void close();

  void terminate();

  void join();

 private:
  void run_task();
  int running_thread_num;
  int thread_num{};
  std::vector<std::thread> ths;
  TaskQueue task_queue_;
  std::mutex new_task_mutex_;
  std::condition_variable new_task;
  std::condition_variable empty_task;
  std::mutex op_mutex_;
  std::condition_variable all_task_done;
  bool wait_to_exit;
};

inline QFThreadPoolExecutor::QFThreadPoolExecutor(int thread_num) {
  std::cout << "Hello, I'm qf_thread_pool_executor" << std::endl;
  std::cout << "thread num = " << thread_num << std::endl;

  this->thread_num = thread_num;
  this->running_thread_num = 0;
  this->wait_to_exit = false;

  for (int i = 0; i < thread_num; i++) {
    this->ths.emplace_back(&QFThreadPoolExecutor::run_task, this);
  }
}

inline QFThreadPoolExecutor::~QFThreadPoolExecutor() {
  std::unique_lock<std::mutex> lock(op_mutex_);
  this->wait_to_exit = true;
  this->new_task.notify_all();
  for(int i = 0; i < thread_num; i ++) {
    ths[i].join();
  }
};

template <typename Func, typename... Args>
void QFThreadPoolExecutor::submit(Func &&func, Args &&...args) {
  std::unique_lock<std::mutex> lock(op_mutex_);
  auto task = Task();
  task.submit(std::ref(func), std::ref(args)...);
  task_queue_.push_task(task);
  new_task.notify_one();
}

inline void QFThreadPoolExecutor::join() {
  std::unique_lock<std::mutex> lock(op_mutex_);

  if(this->task_queue_.empty() && this->running_thread_num == 0) {
    return;
  }

  this->all_task_done.wait(lock);
}

inline void QFThreadPoolExecutor::run_task() {
  while (true) {
    Task task;
    {
      std::unique_lock<std::mutex> lock(new_task_mutex_);
      if (this->task_queue_.empty()) {
        this->new_task.wait(lock);
      }
      if(this->wait_to_exit) {
        break;
      }
      std::cout << "get tak" << std::endl;
      task = this->task_queue_.pop_task();
    }

    {
      std::unique_lock<std::mutex> lock(op_mutex_);
      this->running_thread_num += 1;
    }
    task.execute();
    {
      std::unique_lock<std::mutex> lock(op_mutex_);
      this->running_thread_num -= 1;
      if(this->task_queue_.empty() && this->running_thread_num == 0) {
        this->all_task_done.notify_one();
      }
    }

  }
}

// ------------------------------------------------------------------------- //

#endif  // QF_THREAD_POOL_EXECUTOR_H
