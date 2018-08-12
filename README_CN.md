

#### API Reference

[API Reference
EditNew Page
](https://github.com/Amanieu/asyncplusplus/wiki/API-Reference)

-------

#### Tasks
Task objects是Async++的核心组件。

##### async::spawn
- spawn() 创建任务
```cpp
	auto my_task = async::spawn([] {
		return 41;
	});
```

-  ready() 任务是否完成

-  wait() 等待任务完成

-  get() 等待任务完成并获取返回值

-  then() 延续任务，任务完成后调用
阻塞问题是很难可靠地预测任务何时完成。可以在任务完成后立即使用一个函数来调度一个函数，通过使用then()成员函数完成任务。

```cpp
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
```
- when_any() 任意任务完成时回调

```cpp
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
```

- when_all() 全部任务完毕执行完毕才调用

VS2013编译不过去,VS2017可以。


- cancel() 取消任务

任务取消后，在调用执行将报错。

- share()

正常任务<t>对象是一次性使用的：一旦调用了get()或then()，它们就变成空的，并且对它们的任何进一步操作都是一个错误，除了向它们分配新任务（来自spawn()或从另一个任务对象）。为了多次使用任务，可通过调用share()将任务转换为共享任务。

##### async::event_task

有时有必要等待外部事件发生，或者一些任务组合完成。Assic++允许您通过使用async::event_task<T>类来定义自定义任务对象，这些对象可以任意设置为任意值或异常。通过调用get_task()成员函数，可以检索与事件相关联的任务对象。

- get_task() 获取关联任务

- set(T) 设置信号并执行任务

```cpp
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
```

##### async::local_spawn
有时，最大性能是必需的，并不是所有的特征都由ASYNC ++提供。local_task<F>类提供完全不可复制的、不可移动的任务类型，它们完全驻留在堆栈上。它不需要内存分配，但只支持一组受限制的操作：它不支持连续性和组合，它所允许的唯一操作是get()、waite()和ready()，并且它的析构函数中有一个隐式waite()。

```cpp
auto&& t = async::local_spawn([] {
	std::cout << "Local task" << std::endl;
});
```

-------

#### Parallel algorithms 并行算法

- parallel_invoke 并行调用

并行的方式调用一个函数，底层使用的是async::local_spawn。

```cpp
void example1_paraller_invoke()
{
	// 并行调用
	async::parallel_invoke([] {
		std::cout << "This is executed in parallel..." << std::endl;
	}, [] {
		std::cout << "with this" << std::endl;
	});
}
```



- parallel_for 并行遍历

```cpp
async::parallel_for({ 0, 1, 2, 3, 4 }, [](int x) {
		std::cout << x;
	});
```

- parallel_reduce 元素合并
parallel_reduce()用于将范围的所有元素组合成单个元素。与并行对象一样，范围被分割成块并并行处理。

```cpp
void example3_paraller_reduce()
{
	int r1 = async::parallel_reduce({ 1, 2, 3, 4 }, 0, [](int x, int y) {
		return x + y;
	});
	std::cout << "The sum of {1, 2, 3, 4} is " << r1 << std::endl;
	int r2 = async::parallel_map_reduce({ 1, 2, 3, 4 }, 0, [](int x) {
		return x * 2;
	}, [](int x, int y) {
		return x + y;
	});
	std::cout << "The sum of {1, 2, 3, 4} with each element doubled is " << r2
		<< std::endl;
}
//输出
//The sum of (1,2,3,4) is 10
//The sum of (1,2,3,4) with each element doubled is 20
```


- irange() and make_range()

并行算法只接受范围对象，其具有begin()和end()迭代器成员函数,大多数的C++容器可以直接使用。为了处理其他情况，Assic++提供了两个适配器irange()和make_range()，使范围工作更容易。

```cpp
int gArray[] = { 1, 2, 3, 4, 5 };
int* get_numbers()
{
	return  &gArray[0];
}
size_t get_num_numbers()
{
	return sizeof(gArray) / sizeof(gArray[0]);
}
async::parallel_for(async::irange(0, 5), [](int x) {
	std::cout << x;
});
std::cout << std::endl;

int* numbers = get_numbers();
size_t num_numbers = get_num_numbers();
async::parallel_for(async::make_range(numbers, numbers + num_numbers),
                    [](int x) {
    std::cout << x;
});
std::cout << std::endl;
//输出
//01342
//14325
```

- static_partitioner() and auto_partitioner()

8位一组输出0~1024。

```cpp
// Split the range into chunks of 8 elements or less
async::parallel_for(static_partitioner(async::irange(0, 1024), 8),
	[](int x) {
	std::cout << x << " ";
});
std::cout << std::endl;
```

-------

#### Schedulers 调度者
默认情况下，Async++使用一个工作线程调度器，它带有LIBASYNC_NUM_THREADS（环境变量）线程的线程池（或者如果没有指定系统中的CPU数量）。调度器在第一次使用时被初始化，并且在程序退出时被破坏，同时确保所有当前运行的任务在程序退出之前完成执行。

- default_threadpool_scheduler()

创建默认的调度器，调度器会通过线程池执行任务。

- inline_scheduler()

线程调度器将在当前线程中立即运行任务

- threadpool_scheduler()

调度器将会创建一个新的线程来运行任务。

```cpp
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
```
- Custom schedulers

见例子[examples\gtk_scheduler.cpp](./examples/gtk_scheduler.cpp)
略










