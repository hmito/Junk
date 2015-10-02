#include<chrono>
#include<vector>
#include<iostream>
#include<fstream>
#include<array>
#include<utility>
#include<string>
#include <cstdint>
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

	std::ofstream fout("data_e.csv");
	using type = int64_t;
	std::vector<std::pair<std::string, std::function<void(size_t, type, type)>>> FuncArray;
	FuncArray.push_back(std::make_pair("hash", make_nonrepeat_rand_array_hash<type>));
	FuncArray.push_back(std::make_pair("unique", make_nonrepeat_rand_array_unique<type>));
	FuncArray.push_back(std::make_pair("shuffle", make_nonrepeat_rand_array_shuffle<type>));
	//FuncArray.push_back(std::make_pair("select", make_nonrepeat_rand_array_select<type>));
	FuncArray.push_back(std::make_pair("select with hash", make_nonrepeat_rand_array_select_with_hash<type>));
	FuncArray.push_back(std::make_pair("select with hash no iterator", make_nonrepeat_rand_array_select_with_hash_no_itr<type>));

	try{
		for(size_t array_num : {100,10000,1000000}){
			for(auto func : FuncArray){
				for(type val : {1,2,4,8,16,32,64,128,256}){
					const type rand_max = val*array_num;
					const type rand_min = -val*array_num;
					try {

						std::cout << "array_num : " << array_num << "rand_max : " << rand_max << "rand_min : " << rand_min << std::endl;
						const auto t0 = clock::now();

						for (unsigned int cnt = 0; cnt < 10000000 / array_num; ++cnt) {
							func.second(array_num, rand_min, rand_max);
						}

						auto t1 = clock::now();

						print_elapsed_time(func.first.c_str(), t0, t1);

						auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
						fout << array_num << "," << rand_max << "," << rand_min << "," << func.first << "," << time.count() << std::endl;
						//if(time.count() > 7500)break;
					}
					catch (const std::exception& er) {
						std::cerr << er.what() << std::endl;
						fout << array_num << "," << rand_max << "," << rand_min << "," << func.first << "," << er.what() << std::endl;
					}
				}
			}
		}
	}
	catch(const std::exception& er){
		std::cerr << er.what() << std::endl;
	}

#ifdef _MSC_VER
	system("pause");//mingw gcc環境では必要ない
#endif
	return 0;
}

