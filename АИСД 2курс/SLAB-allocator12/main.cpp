#include <cstdlib>
#include <algorithm>
#include "Slab.h"
#include <chrono>
#include <iostream>

#define N 1000000
using namespace std::chrono;
const int kMillion = 1e6;

template<typename Func>
long long calculate_execution_time(Func funcion) {
	const size_t iterations = 1000;
	long long duration = 0;
	for (size_t i = 0; i < iterations; i++) {
		auto start = high_resolution_clock::now();
		funcion();
		auto stop = high_resolution_clock::now();
		duration += duration_cast<nanoseconds>(stop - start).count();
	}
	return duration / iterations;
}
void TestTimeSlab(size_t allocation_size) {
	const int num = 10000;
	cache cache;
	cache_setup(&cache, allocation_size);
	void* addresses[num];
	auto start1 = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < num; i++) {
		addresses[i] = cache_alloc(&cache);
	}
	auto end1 = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < num; i++) {
		cache_free(&cache, addresses[i]);
	}

	cache_shrink(&cache);
	auto dur1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
	std::cout << "Time taken by slab to allocate: " << dur1.count() << " microseconds" << std::endl;
}

void TestTimeMalloc(size_t allocation_size) {
	const int num = 10000;
	void* addresses[num];
	auto start1 = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < num; i++) {
		addresses[i] = malloc(allocation_size);
	}
	auto end1 = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < num; i++) {
		free(addresses[i]);
	}

	auto dur1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
	std::cout << "Time taken by malloc to allocate: " << dur1.count() << " microseconds" << std::endl;
}
void benchmark_allocation_size(size_t allocation_size) {
	printf("For size \n");
	std::cout << allocation_size;
	TestTimeSlab(allocation_size);
	TestTimeMalloc(allocation_size);


	//if (my_malloc_time > malloc_time) {
	//	printf("custom_malloc is %.2f times slower than malloc\n", (double)my_malloc_time / malloc_time);
	//}
	//else {
	//	printf("custom_malloc is %.2f times faster than malloc\n", (double)malloc_time / my_malloc_time);
	//}
	printf("------------------------------------------------\n");
}

void Test1() {
	cache cache;

	cache_setup(&cache, 1024 * 8 * 1024);
	void* ptr1 = cache_alloc(&cache);
	void* ptr2 = cache_alloc(&cache);
	cache_free(&cache, ptr2);
	cache_free(&cache, ptr1);
	cache_release(&cache);
	cache_info(&cache);

}
void Test2() {
	cache cache;
	void* ptr1, * ptr2, * ptr3;
	cache_setup(&cache, 2000000);
	ptr1 = cache_alloc(&cache);
	ptr2 = cache_alloc(&cache);
	cache_info(&cache);
	slab_info(&cache);
	cache_free(&cache, ptr2);
	cache_info(&cache);
	cache_shrink(&cache);


}
void Test3() {
	cache cache;

	cache_setup(&cache, 2000000);
	for (int i = 0; i < 32; i++) {
		void* ptr1 = cache_alloc(&cache);
		slab_info(&cache);
	}
	cache_info(&cache);

}

int main()
{
	//benchmark_allocation_size(16);
	//benchmark_allocation_size(512);
	//benchmark_allocation_size(4096);
	//benchmark_allocation_size(32768);

	//test();
	Test2();


	return 0;
}