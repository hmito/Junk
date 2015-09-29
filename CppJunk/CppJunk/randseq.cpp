#include <chrono>
#include <utility>
#include <string>
#include"randseq.hpp"

template<class T>
void print_elapsed_time(const std::string& str, T start, T end){
	std::cout << str << " : "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
		<< " msec."
		<< std::endl;
}
int main(){
	using clock = std::chrono::high_resolution_clock;
	std::vector<std::pair<std::string, std::function<std::vector<int>(size_t, int, int)>>> FuncArray;

	FuncArray.emplace_back("make_rand_array_hash", make_rand_array_hash);
	FuncArray.emplace_back("make_rand_array_unique", make_rand_array_unique);
	//FuncArray.emplace_back("make_rand_array_shuffle", make_rand_array_shuffle);
	FuncArray.emplace_back("make_rand_array_select", make_rand_array_select);
	FuncArray.emplace_back("make_rand_array", make_rand_array);

	try{
		for(auto val : {1, 5, 10, 18, 19, 20, 100, 500, 1000, 10000}){
			constexpr size_t array_num = 10000;
			const int rand_max = val*array_num;
			const int rand_min = -val*array_num;

			std::cout << "array_num : " << array_num << " rand_max : " << rand_max << " rand_min : " << rand_min << std::endl;
			for(auto func : FuncArray){
				const auto t0 = clock::now();

				for(size_t cnt = 0; cnt < 100; ++cnt){
					func.second(array_num, rand_min, rand_max);
				}

				print_elapsed_time(func.first.c_str(), t0, clock::now());
			}
		}
	}
	catch(const std::exception& er){
		std::cerr << er.what() << std::endl;
	}

#ifdef _WIN32
	system("pause");
#endif
	return 0;
}

