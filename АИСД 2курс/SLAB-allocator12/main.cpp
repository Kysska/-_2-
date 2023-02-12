#include <cstdlib>
#include <algorithm>
#include "Slab.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <ctime>

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
void generation_test(int count_test) {
	for (int i = 1; i <= count_test; i++) {
		cache cache;
		std::map <int, void*> mp;
		std::string numtest = std::to_string(i);
		std::ofstream fin("./tests/generation_test/input_" + std::to_string(i) + ".in");
		std::ofstream fout("./tests/generation_test/output_" + std::to_string(i) + ".out");
		srand((unsigned int)time(NULL));
		int a = rand() % 4000000 + 16; //размер кэша
		cache_setup(&cache, a);
		std::cout << a << " ";
		fin << a << "\n";
		int b = rand() % 1000 + 1; //количество операций
		std::cout << b << " ";
		for (int i = 1; i <= b; i++) {
			int c = rand() % 2 + 1;
			std::cout << c << " ";
			if (c == 1) {
				fin << "alloc(" << i << ")\n";
				void * ptr1 = cache_alloc(&cache);
				mp[i] = ptr1;
				std::cout << mp[i] << " ";
			}
			else if (c == 2) {
				std::cout << mp.size() << " ";
				if (mp.size() == 0) {
					continue;
				}
				int d = rand() % mp.size();
				std::cout << d << " ";
				fin << "free(" << d << ")\n";
				
				cache_free(&cache, mp[d]);				
				std::cout << mp[d] << " ";
				mp.erase(d);

			}
		}
		cache_info_infile(&cache, i);
		cache_release(&cache);
		fin.close();
		fout.close();
	}
}
void Test(int count_test) {
	
	for (int i = 1; i <= count_test; i++) {
		cache cache;
		std::map <char, void*> mp;
		std::string numtest = std::to_string(i);
		std::ifstream fin("./tests/" + std::to_string(i) + ".in");
		std::ofstream fout("./tests/" + std::to_string(i) + ".out");

		std::string line;
		std::string allocsize;
		bool flag = false;
		if (fin.is_open()) {
			getline(fin, allocsize);
			std::istringstream iss(allocsize);
			size_t size;
			iss >> size;
			if (size <= 0) {
				size = 16;
			}
			else if (size > 4000000) {
				size = 4000000;
			}
			cache_setup(&cache, size);
		}
		else {
			std::cout << i <<" test : Error: The file does not open" << std::endl;
			continue;
		}

		if (fin.is_open()) {
			auto start1 = std::chrono::high_resolution_clock::now();
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
							flag = true;
							std::cout << "test " << i << ": error in execution. Error: unidentified problem " << std::endl;
							break;
						}
						
					}
				}
				else if(line.find("free") != std::string::npos) {
					size_t found = line.find("(");
					if (found != std::string::npos) {
						char countfree = line[found + 1];
						if (mp.count(countfree) != 0) {
							void* ptr1 = mp[countfree];
							try {
								if (ptr1 == NULL) {
									throw "Error: ptr is null";
								}
								cache_free(&cache, ptr1);
							}
							catch (const char* e) {
								flag = true;
								std::cout << "test " << i << ": error in execution. " << e << std::endl;
								break;
							}
							catch (...) {
								flag = true;
								std::cout << "test " << i << ": error in execution. Error: unidentified problem " << std::endl;
								break;
							}
						}
						else continue;
					}
				}
				else {
					try {
						cache_release(&cache);
					}
					catch (...) {
						flag = true;
						std::cout << "test " << i << ": error in execution. Error: unidentified problem " << std::endl;
						break;
					}
				}
			}
			if (flag == false) {
               cache_info_infile(&cache, i);
			   auto end1 = std::chrono::high_resolution_clock::now();
			   auto dur1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
			   std::cout << "test " << i << ": successful. time of work:" << dur1.count() << " ms." << std::endl;
			}	
		    cache_release(&cache);
		}
		fin.close();
		fout.close();
	}

}

void Test1() {
	cache cache;
	std::stringstream stream;
	cache_setup(&cache, 192);
	void* ptr1 = cache_alloc(&cache);
	void* ptr2 = cache_alloc(&cache);
	cache_free(&cache, ptr2);
	//cache_free(&cache, ptr1);
	//cache_release(&cache);
	cache_info(&cache);


}
void Test2() {
	cache cache;
	void* ptr1, * ptr2, * ptr3, *ptr4, *ptr5, *ptr6;
	cache_setup(&cache, 335);
	ptr1 = cache_alloc(&cache);
	ptr2 = cache_alloc(&cache);
	cache_free(&cache, ptr1);
	cache_info(&cache);
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

	//Test1();
	/*Test(12);*/
	generation_test(1);

	return 0;
}