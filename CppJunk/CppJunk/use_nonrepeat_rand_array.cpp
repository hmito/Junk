#include<chrono>
#include<vector>
#include<iostream>
#include<array>
#include<utility>
#include<string>
#include"nonrepeat_rand_array.hpp"

template<class T>
void print_elapsed_time(const char* str, T start, T end){
	std::cout << str << " : "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
		<< " msec."
		<< std::endl;
}
using make_rand_array_type = std::vector<int>(*)(size_t, int, int);
int main(){
	using clock = std::chrono::high_resolution_clock;
	size_t array_num = 10000;
	int rand_max = 1000000;
	int rand_min = -1000000;

	std::vector<std::pair<std::string, std::function<void(size_t, int, int)>>> FuncArray;

	FuncArray.push_back(std::make_pair("make_nonrepeat_rand_array_hash", make_nonrepeat_rand_array_hash<int>));
	FuncArray.push_back(std::make_pair("make_nonrepeat_rand_array_unique", make_nonrepeat_rand_array_unique<int>));
	FuncArray.push_back(std::make_pair("make_nonrepeat_rand_array_select", make_nonrepeat_rand_array_select<int>));

	try{
		for(size_t array_num_ : {100,10000,1000000}){
			for(int val : {1,10,100,1000,10000,100000}){
				array_num = array_num_;
				rand_max = array_num+val;
				rand_min = 0;

				std::cout << "array_num : " << array_num << "rand_max : " << rand_max << "rand_min : " << rand_min << std::endl;
				for(auto func : FuncArray){
					const auto t0 = clock::now();

					for(unsigned int cnt = 0; cnt < 10000000 / array_num; ++cnt){
						func.second(array_num, rand_min, rand_max);
					}

					auto t1 = clock::now();

					print_elapsed_time(func.first.c_str(), t0, t1);
				}
			}
		}
	}
	catch(const std::exception& er){
		std::cerr << er.what() << std::endl;
	}


	system("pause");

	return 0;
}

