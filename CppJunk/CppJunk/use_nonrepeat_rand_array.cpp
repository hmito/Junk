#include<chrono>
#include<vector>
#include<iostream>
#include<fstream>
#include<cmath>
#include<array>
#include<utility>
#include<string>
#include <cstdint>
#include"nonrepeat_rand_array.hpp"

using make_rand_array_type = std::vector<int>(*)(size_t, int, int);
int main(){
	using namespace std::chrono;

	std::ofstream fout("data_151002_b.csv");

	std::vector<std::pair<std::string, std::function<void(size_t, int, int)>>> FuncArray;
	FuncArray.push_back(std::make_pair("hash", make_nonrepeat_rand_array_hash<int>));
	FuncArray.push_back(std::make_pair("unique", make_nonrepeat_rand_array_unique<int>));
//	FuncArray.push_back(std::make_pair("shuffle", make_nonrepeat_rand_array_shuffle<int>));
//	FuncArray.push_back(std::make_pair("select", make_nonrepeat_rand_array_select<int>));
	FuncArray.push_back(std::make_pair("select with hash", make_nonrepeat_rand_array_select_with_hash<int>));


	try{
		for(auto func : FuncArray){
			for(size_t array_num : {100,10000,1000000}){
				for(int val = 0; val < 30; ++val){
					const auto rand_max = static_cast<int>(array_num * std::pow(10.0, val/10.0));
					decltype(rand_max) rand_min = 0;
					try{
						std::cout << "array_num : " << array_num << " rand_max : " << rand_max << " rand_min : " << rand_min << " type : "<<func.first;
						const auto t0 = high_resolution_clock::now();

							for (unsigned int cnt = 0; cnt < 10000000 / array_num; ++cnt) {
							func.second(array_num, rand_min, rand_max);
						}

						const auto t1 = high_resolution_clock::now();
						const auto time = duration_cast<milliseconds>(t1 - t0);

						std::cout << " = " << time.count() << "msec" << std::endl;
						fout << array_num << "," << rand_max << "," << rand_min << "," << func.first << "," << time.count() << std::endl;
						//if(time.count() > 7500)break;
					}
					catch (const std::exception& er) {
						std::cerr << er.what() << std::endl;
						//fout << array_num << "," << rand_max << "," << rand_min << "," << func.first << "," << er.what() << std::endl;
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

