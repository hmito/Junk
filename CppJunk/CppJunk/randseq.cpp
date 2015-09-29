#include<chrono>
#include<vector>
#include<array>
#include<utility>
#include<fstream>
#include<string>
#include"randseq.hpp"

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

	FuncArray.push_back(std::make_pair("make_rand_array_just_shuffle", make_rand_array_just_shuffle));
	FuncArray.push_back(std::make_pair("make_rand_array_shuffle", make_rand_array_shuffle));
	FuncArray.push_back(std::make_pair("make_rand_array_select", make_rand_array_select));

	try{
		std::ofstream fout("randseq3.csv");

		fout << "array_num, rand_max, rand_min, type, time\n";
		for(size_t array_num_ : {10000}){
			for(int val : {5000,5001,5002,5004,5008,5016}){
				array_num = array_num_;
				rand_max = val;
				rand_min = -val;

				std::cout << "array_num : " << array_num << "rand_max : " << rand_max << "rand_min : " << rand_min << std::endl;
				for(auto func : FuncArray){
					const auto t0 = clock::now();

					for(unsigned int cnt = 0; cnt < 10000000 / array_num; ++cnt){
						func.second(array_num, rand_min, rand_max);
					}

					auto t1 = clock::now();

					print_elapsed_time(func.first.c_str(), t0, t1);

					fout << array_num << "," << rand_max << "," << rand_min << "," << func.first << "," << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << std::endl;
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

