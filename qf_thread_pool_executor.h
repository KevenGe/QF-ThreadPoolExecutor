//
// Created by QF on 5/8/2024.
//

#ifndef QF_THREAD_POOL_EXECUTOR_H
#define QF_THREAD_POOL_EXECUTOR_H

#include <atomic>
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

  void execute() const;

 private:
  std::function<void()> func;
};

template <typename Func, typename... Args>
void Task::submit(Func &&func, Args &&...args) {
  auto bind_func =
      std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
  this->func = [bind_func]() { bind_func(); };
}

inline void Task::execute() const { this->func(); }

// ------------------------------------------------------------------------- //

class TaskQueue {
 public:
  TaskQueue();

  ~TaskQueue();

  bool empty() const;

  unsigned long long size() const;

  void push_task(const Task &task);

  Task pop_task();

 private:
  std::mutex op_mutex;
  std::queue<Task> que;
};

inline TaskQueue::TaskQueue() { this->que = std::queue<Task>{}; }

inline TaskQueue::~TaskQueue() = default;

inline bool TaskQueue::empty() const {
  std::unique_lock<std::mutex>(op_mutex);
  return que.empty();
}

inline unsigned long long TaskQueue::size() const {
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

  // thread number
  int thread_num;

  // task queue
  TaskQueue task_queue;

  // workers
  std::vector<std::thread> workers;

  // atomic int, the number of running thread
  std::atomic<int> running_thread_num;

  // wait to exit
  std::atomic<bool> wait_to_exit;

  // operation mutex
  std::mutex op_mutex;

  // the mutex of get the new task
  std::mutex get_new_task_mutex;

  // the condition_variable of the conition of the worker need wake
  std::condition_variable worker_need_wake;

  //
  std::condition_variable all_tasks_done;
};

inline QFThreadPoolExecutor::QFThreadPoolExecutor(int thread_num) {
  std::cout << "thread num = " << thread_num << std::endl;

  this->thread_num = thread_num;
  this->running_thread_num.store(0);
  this->wait_to_exit.store(false);

  // create threads
  for (int i = 0; i < thread_num; i++) {
    this->workers.emplace_back(&QFThreadPoolExecutor::run_task, this);
  }
}

inline QFThreadPoolExecutor::~QFThreadPoolExecutor() {
  std::unique_lock<std::mutex> lock(op_mutex);
  this->wait_to_exit.store(true);
  this->worker_need_wake.notify_all();
  for (auto &worker : workers) {
    worker.join();
  }
};

template <typename Func, typename... Args>
void QFThreadPoolExecutor::submit(Func &&func, Args &&...args) {
  std::unique_lock<std::mutex> lock(op_mutex);
  auto task = Task();
  task.submit(std::ref(func), std::ref(args)...);
  task_queue.push_task(task);
  worker_need_wake.notify_one();
}

inline void QFThreadPoolExecutor::join() {
  std::unique_lock<std::mutex> lock(op_mutex);
  if (this->task_queue.empty() && this->running_thread_num.load() == 0) {
    return;
  }
  this->all_tasks_done.wait(lock);
}

inline void QFThreadPoolExecutor::run_task() {
  // endless run
  while (true) {
    Task task;
    {
      std::unique_lock<std::mutex> lock(get_new_task_mutex);
      if (this->task_queue.empty()) {
        this->worker_need_wake.wait(lock);
      }
      if (this->wait_to_exit.load()) {
        break;
      }
      std::cout << "get task" << std::endl;
      task = std::move(this->task_queue.pop_task());
    }

    // run and cal the number of running threads
    this->running_thread_num.fetch_add(1);
    std::cout << std::hash<std::thread::id>{}(std::this_thread::get_id())
              << std::endl;
    task.execute();
    this->running_thread_num.fetch_sub(1);

    //
    if (this->task_queue.empty() && this->running_thread_num.load() == 0) {
      this->all_tasks_done.notify_one();
    }
  }
}

// ------------------------------------------------------------------------- //

#endif  // QF_THREAD_POOL_EXECUTOR_H
