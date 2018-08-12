#include <iostream>
#include <async++.h>
#ifdef _DEBUG
#pragma comment(lib,"../debug/Async++.lib")
#else
#pragma comment(lib,"../release/Async++.lib")
#endif // _DEBUG


void example1()
{
	// Spawn a task in a new thread
	// 立即创建一个线程
	auto t = async::spawn(async::thread_scheduler(), [] {
		std::cout << "Running in a separate thread!" << std::endl;
	});

	// Create a new thread pool with 10 threads  
	// 立即创建一个线程
	async::threadpool_scheduler custom_pool(10);

	// Spawn a continuation in the thread pool
	t.then(custom_pool, [] {
		std::cout << "Running a continuation in inline scheduler!" << std::endl;
	});

	// Create a FIFO scheduler
	async::fifo_scheduler fifo;

	// Queue a local_task in the FIFO
	auto&& t2 = async::local_spawn(fifo, [] {
		std::cout << "Running a local task from the queue!" << std::endl;
	});

	// Execute all tasks queued in the FIFO in the current thread
	// 创建10个线程
	fifo.run_all_tasks();
}

// Wait handler that disallows blocking on the UI thread. This means that
// you must check that a task has completed before calling get().
void ui_wait_handler(async::task_wait_handle)
{
	std::cerr << "Error: Blocking wait in UI thread" << std::endl;
	std::abort();
}

#include <condition_variable>
// Generic wait handler that sleeps until the task has completed
void sleep_wait_handler(async::task_wait_handle t)
{
	std::condition_variable cvar;
	std::mutex lock;
	bool done = false;

	t.on_finish([&] {
		std::lock_guard<std::mutex> locked(lock);
		done = true;
		cvar.notify_one();
	});

	std::unique_lock<std::mutex> locked(lock);
	while (!done)
		cvar.wait(locked);
}

void example2()
{
	// Set wait handler on GTK thread to disallow waiting for tasks
	async::set_thread_wait_handler(ui_wait_handler);


}

int main()
{
	example1();
	auto t1 = async::spawn([] {
		std::cout << "OK" << std::endl;
	});
	auto t2 = async::make_task(42);
	return 0;
}
