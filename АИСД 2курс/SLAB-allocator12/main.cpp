#include <cstdlib>
#include <algorithm>
#include "Slab.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

#define N 1000000
using namespace std::chrono;
const int kMillion = 1e6;

void TestTimeSlab(size_t allocation_size) {
	const int num = 10000;
	cache cache;
	cache_setup(&cache, allocation_size);
	void* addresses[num];

	for (size_t i = 0; i < num; i++) {
		addresses[i] = cache_alloc(&cache);
	}
	auto start1 = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < num; i++) {
		cache_free(&cache, addresses[i]);
	}
	auto end1 = std::chrono::high_resolution_clock::now();
	cache_shrink(&cache);
	auto dur1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
	std::cout << "Time taken by slab to allocate: " << dur1.count() << " microseconds" << std::endl;
}

void TestTimeMalloc(size_t allocation_size) {
	const int num = 10000;
	void* addresses[num];

	for (size_t i = 0; i < num; i++) {
		addresses[i] = malloc(allocation_size);
	}
	auto start1 = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < num; i++) {
		free(addresses[i]);
	}
	auto end1 = std::chrono::high_resolution_clock::now();
	auto dur1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
	std::cout << "Time taken by malloc to allocate: " << dur1.count() << " microseconds" << std::endl;
}
void benchmark_allocation_size(size_t allocation_size) {
	printf("For size \n");
	std::cout << allocation_size;
	TestTimeSlab(allocation_size);
	TestTimeMalloc(allocation_size);



	printf("------------------------------------------------\n");
}
void print_state(const std::ios& stream) {
	std::cout << " good()=" << stream.good();
	std::cout << " eof()=" << stream.eof();
	std::cout << " fail()=" << stream.fail();
	std::cout << " bad()=" << stream.bad();
}
void Test(int count_test) {
	int ok = 0;
	int wa = 0;
	std::map <char, void*> mp;
	for (int i = 1; i <= count_test; i++) {
		cache cache;
		std::string numtest = std::to_string(i);
		std::ifstream fin("./tests/" + std::to_string(i) + ".in");
		std::ofstream fout("./tests/" + std::to_string(i) + ".out");

		std::string line;
		std::string allocsize;
		getline(fin, allocsize);
		std::istringstream iss(allocsize);
		size_t size;
		iss >> size;
		if (size > 40000000) {
			size = 4000000;
		}
		else if (size <= 0) {
			size = 16;
		}
		cache_setup(&cache, size);
		if (fin.is_open()) {
			while (getline(fin, line)) {
				if (line.find("alloc") != std::string::npos) {
					size_t found = line.find("(");
					if (found != std::string::npos) {
						char countalloc = line[found + 1];
						try {
							void* ptr = cache_alloc(&cache);
							mp[countalloc] = ptr;
						}
						catch (...) {
							std::cout << "Ошибка в освобождение";
						}

					}
				}
				else if(line.find("free") != std::string::npos) {
					size_t found = line.find("(");
					if (found != std::string::npos) {
						char countfree = line[found + 1];
						if (mp.count(countfree) != 0) {
							void* ptr1 = mp[countfree];
							if (ptr1 == nullptr) {
								std::cout << "Nullptr" << std::endl;
							}
							else cache_free(&cache, ptr1);

						}
						else continue;
					}
				}
				else {
					cache_release(&cache);
				}
			}
		}

		cache_info_infile(&cache, i);
		fin.close();
		fout.close();
	}

}

void Test1() {
	cache cache;
	std::stringstream stream;
	cache_setup(&cache, 1024 * 8 * 1024);
	void* ptr1 = cache_alloc(&cache);
	void* ptr2 = cache_alloc(&cache);
	cache_free(&cache, ptr2);
	//cache_free(&cache, ptr1);
	//cache_release(&cache);
	cache_info(&cache);


}
void Test2() {
	cache cache;
	void* ptr1, * ptr2, * ptr3;
	cache_setup(&cache, 3);
	ptr1 = cache_alloc(&cache);
	ptr2 = cache_alloc(&cache);
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

	//Test(10);
	Test(10);

	return 0;
}