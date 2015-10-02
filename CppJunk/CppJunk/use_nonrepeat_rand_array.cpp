#include<chrono>
#include<vector>
#include<iostream>
#include<fstream>
#include<cmath>
#include<array>
#include<utility>
#include<string>
#include"nonrepeat_rand_array.hpp"

using make_rand_array_type = std::vector<int>(*)(size_t, int, int);
int main(){
	using clock = std::chrono::high_resolution_clock;
	size_t array_num = 10000;
	int rand_max = 1000000;
	int rand_min = -1000000;

	std::ofstream fout("data_151002_s.csv");

	std::vector<std::pair<std::string, std::function<void(size_t, int, int)>>> FuncArray;
//	FuncArray.push_back(std::make_pair("hash", make_nonrepeat_rand_array_hash<int>));
//	FuncArray.push_back(std::make_pair("unique", make_nonrepeat_rand_array_unique<int>));
//	FuncArray.push_back(std::make_pair("shuffle", make_nonrepeat_rand_array_shuffle<int>));
//	FuncArray.push_back(std::make_pair("select", make_nonrepeat_rand_array_select<int>));
//	FuncArray.push_back(std::make_pair("select with hash", make_nonrepeat_rand_array_select_with_hash<int>));
	FuncArray.push_back(std::make_pair("unique1", make_nonrepeat_rand_array_unique1<int>));
	FuncArray.push_back(std::make_pair("unique2", make_nonrepeat_rand_array_unique2<int>));
	FuncArray.push_back(std::make_pair("unique3", make_nonrepeat_rand_array_unique3<int>));

	try{
		for(auto func : FuncArray){
			for(size_t array_num_ : {100,10000,1000000}){
				for(int val = 0; val < 30; ++val){
					array_num = array_num_;
					rand_max = static_cast<int>(array_num_ * std::pow(10.0, val/10.));
					rand_min = 0;

					std::cout << "array_num : " << array_num << " rand_max : " << rand_max << " rand_min : " << rand_min << " type : "<<func.first;
					const auto t0 = clock::now();

					for(unsigned int cnt = 0; cnt < 10000000 / array_num; ++cnt){
						func.second(array_num, rand_min, rand_max);
					}

					auto t1 = clock::now();

					auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

					std::cout << " = " << time.count() << "msec" << std::endl;
					fout << array_num << "," << rand_max << "," << rand_min << "," << func.first << "," << time.count() << std::endl;

//					if(time.count() > 7500)break;
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

