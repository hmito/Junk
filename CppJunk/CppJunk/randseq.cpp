#include<chrono>
#include<vector>
#include<array>
#include<utility>
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

	FuncArray.push_back(std::make_pair("make_rand_array_hash", make_rand_array_hash));
	FuncArray.push_back(std::make_pair("make_rand_array_unique", make_rand_array_unique));
//	FuncArray.push_back(std::make_pair("make_rand_array_shuffle", make_rand_array_shuffle));

	try{
		for(auto val : {10,100,1000,10000}){
			array_num = 10000;
			rand_max = val*array_num;
			rand_min = -val*array_num;

			std::cout << "array_num : " << array_num << "rand_max : " << rand_max << "rand_min : " << rand_min << std::endl;
			for(auto func : FuncArray){
				const auto t0 = clock::now();

				for(unsigned int cnt = 0; cnt < 100;++cnt){
					func.second(array_num, rand_min, rand_max);
				}

				print_elapsed_time(func.first.c_str(), t0, clock::now());
			}
		}
	}
	catch(const std::exception& er){
		std::cerr << er.what() << std::endl;
	}


	system("pause");

	return 0;
}

