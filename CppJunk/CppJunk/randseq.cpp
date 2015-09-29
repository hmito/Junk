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
int main(){
	using clock = std::chrono::high_resolution_clock;
	size_t array_num = 10000;
	int rand_max = 1000000;
	int rand_min = -1000000;

	std::vector<std::pair<std::string, std::function<std::vector<int>(size_t, int, int)>>> FuncArray;

	FuncArray.emplace_back("make_rand_array_hash", make_rand_array_hash);
	FuncArray.emplace_back("make_rand_array_unique", make_rand_array_unique);
	//FuncArray.emplace_back("make_rand_array_shuffle", make_rand_array_shuffle);
	FuncArray.emplace_back("make_rand_array_liner_gen_shuffle", make_rand_array_liner_gen_shuffle);

	try{
		{
			const auto a = make_rand_array_liner_gen_shuffle(24, 150, -150);
			for (auto i : a) std::cout << i << ", ";
			std::cout << std::endl << std::endl;
			system("pause");
		}
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

