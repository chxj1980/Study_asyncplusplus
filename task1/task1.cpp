#include <iostream>
#include <async++.h>
#ifdef _DEBUG
#pragma comment(lib,"../debug/Async++.lib")
#else
#pragma comment(lib,"../release/Async++.lib")
#endif // _DEBUG


void example1()
{
	// Create a task which runs asynchronously
	auto my_task = async::spawn([] {
		return 41;
	});

	// Do other stuff while the task is not finished
	while (!my_task.ready()) {
		// Do stuff...
		std::cout << "等待任务OK" << std::endl;
	}

	// Wait for the task to complete without getting the result
	my_task.wait();

	// Wait for the task to complete and get the result
	int answer = my_task.get();

	// Create a task with a preset value
	auto my_task2 = async::make_task(42);

	// Print the value stored in the task
	std::cout << my_task2.get() << std::endl;
}


void example2()
{
	// Spawn a task
	auto t1 = async::spawn([] {
		return 43;
	});

	// Chain a value-based continuation
	auto t2 = t1.then([](int result) {
		return result;
	});

	// Chain a task-based continuation
	t2.then([](async::task<int> parent) {
		std::cout << parent.get() << std::endl;
	});

	// Equivalent code with direct chaining
	auto t3 = async::spawn([] {
		return 44;
	}).then([](int result) {
		return result;
	}).then([](async::task<int> parent) {
		std::cout << parent.get() << std::endl;
	});

	// 添加等待执行完成，避免程序崩溃
	//t2.get();
	//t3.get();
}

void example3()
{
	// Using when_any to find task which finishes first
	async::task<char> tasks[] = {
		async::spawn([] {return 'A'; }),
		async::spawn([] {return 'B'; }),
		async::spawn([] {return 'C'; })
	};
	async::when_any(tasks).then(
		[](async::when_any_result<std::vector<async::task<char>>> result) {
		std::cout << "Task " << result.index << " finished with value "
			<< result.tasks[result.index].get() << std::endl;
	});

	// Using when_all to combine results of multiple tasks
	auto a = async::spawn([] {return std::string("Hello "); });
	auto b = async::spawn([] {return std::string("World!"); });
#if _MSC_VER <=1800
	std::cout << "VS 版本不支持" << std::endl;
#else
	async::when_all(a, b).then(
		[](std::tuple<async::task<std::string>, async::task<std::string>> result) {
		std::cout << std::get<0>(result).get().c_str() << std::get<1>(result).get().c_str() << std::endl;
	}
	);
#endif // _MSC_VER <=1800

}

void example4()
{
#if _MSC_VER > 1800 
	async::spawn([] {
		throw std::runtime_error("Some error");
	}).then([](int result) {
		// This is not executed because it is a value-based continuation
	}).then([](async::task<void> t) {
		// The exception from the parent task is propagated through value-based
		// continuations and caught in task-based continuations.
		try {
			t.get();
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
		}
	});
#endif //  _MSC_VER > 1800
}

// 取消任务
void example5_cancel_task()
{
	// Create a token
	async::cancellation_token c;

	auto t = async::spawn([&c] { // Capture a reference to the token
		// Throw an exception if the task has been canceled
		async::interruption_point(c);

		// This is equivalent to the following:
		// if (c.is_canceled())
		//     async::cancel_current_task(); // throws async::task_canceled
	});

	// Set the token to cancel work
	c.cancel();

	// Because the task and c.cancel() are executing concurrently, the token
	// may or may not be canceled by the time the task reaches the interruption
	// point. Depending on which comes first, this may throw a
	// async::task_canceled exception.
	t.get();
}

// 共享任务
void example6_share_task()
{
	// Parent task, note the use of .shared() to get a shared task
	auto t = async::spawn([] {
		std::cout << "Parent task" << std::endl;
	}).share();

	// First child, using value-based continuation
	t.then([] {
		std::cout << "Child task A" << std::endl;
	});

	// Second child, using task-based continuation
	t.then([](async::shared_task<void>) {
		std::cout << "Child task B" << std::endl;
	});
}

// 任务解包
void example7_unwrap_task()
{
	// The outer task is task<int>, and its result is set when the inner task
	// completes
	async::spawn([] {
		std::cout << "Outer task" << std::endl;
		// Return a task<int>
		return async::spawn([] {
			std::cout << "Inner task" << std::endl;
			return 42;
		});
	}).then([](int result) {
		std::cout << "Continuation task" << std::endl;
		std::cout << result << std::endl;
	});

	// Output:
	// Outer task
	// Inner task
	// Continuation task
	// 42
}

void example8_Event_tasks()
{
	// Create an event
	async::event_task<int> e;

	// Get a task associated with the event
	auto t = e.get_task();

	// Add a continuation to the task
	t.then([](int result) {
		std::cout << result << std::endl;
	});

	// Set the event value, which will cause the continuation to run
	e.set(42);

	// To set an exception:
	// e.cancel();
	// e.set_exception(std::make_exception_ptr(async::task_canceled));
	// These are equivalent but cancel is slightly more efficient
}


#include <Windows.h>
// 最大性能，只能get(), wait() and ready()接口 
// 完成是栈模式,析构函数中有一个隐式waite()。
// 类似boost::scoped_ptr
void example9_lock_task()
{
	auto&& t = async::local_spawn([] {
		std::cout << "Local task" << std::endl;
	});
	// 取消不了
	t.canceled();
	
}


int main()
{	
	example1();
	example2();
	example3();
	example4();
	try
	{
		example5_cancel_task();
	}
	catch (...)
	{
		std::cout << "任务取消了" << std::endl;
	}
	example6_share_task();
	example7_unwrap_task();
	example8_Event_tasks();
	example9_lock_task();
	// 给异步任务运行时间
	Sleep(1000);
	return 0;
}

