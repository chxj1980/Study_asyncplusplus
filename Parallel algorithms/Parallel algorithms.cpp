#include <iostream>
#include <async++.h>
#ifdef _DEBUG
#pragma comment(lib,"../debug/Async++.lib")
#else
#pragma comment(lib,"../release/Async++.lib")
#endif // _DEBUG

void example1_paraller_invoke()
{
	// 并行调用
	async::parallel_invoke([] {
		std::cout << "This is executed in parallel..." << std::endl;
	}, [] {
		std::cout << "with this" << std::endl;
	});
}

void example2_paraller_for()
{
	async::parallel_for({ 0, 1, 2, 3, 4 }, [](int x) {
		std::cout << x;
	});
	std::cout << std::endl;
}

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

int gArray[] = { 1, 2, 3, 4, 5 };

int* get_numbers()
{
	return  &gArray[0];
}

size_t get_num_numbers()
{
	return sizeof(gArray) / sizeof(gArray[0]);
}

void example4_make_range()
{
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
}


// 变形
void example5_static_partitioner()
{
	// Split the range into chunks of 8 elements or less
	async::parallel_for(static_partitioner(async::irange(0, 1024), 8),
		[](int x) {
		std::cout << x << " ";
	});
	std::cout << std::endl;
}

#include <Windows.h>
int main()
{
	example1_paraller_invoke();
	example2_paraller_for();
	example3_paraller_reduce();
	example4_make_range();
	example5_static_partitioner();


	Sleep(1000);
 	return 0;
} 