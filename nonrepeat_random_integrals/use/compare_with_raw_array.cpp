#include<chrono>
#include<vector>
#include<iostream>
#include<fstream>
#include<cmath>
#include<array>
#include<utility>
#include<string>
#include<algorithm>
#include<numeric>
#include <cstdint>
#include"../../nonrepeat_random_integrals/use/nonrepeat_rand_array.hpp"

int main() {
	using namespace std::chrono;

	constexpr unsigned int array_num = 100000;
	int rand_min = 0;
	int rand_max = 1000000;

	{
		std::vector<int> Ans;
		std::cout << "array_num : " << array_num << " rand_max : " << rand_max << " rand_min : " << rand_min << " type : " << "select";
		const auto t0 = high_resolution_clock::now();

		Ans = make_nonrepeat_rand_array_select(array_num, rand_min, rand_max);

		const auto t1 = high_resolution_clock::now();
		const auto time = duration_cast<milliseconds>(t1 - t0);

		std::cout << " = " << time.count() << "msec" << std::endl;
	}

	{
		std::vector<int> Ans;
		std::cout << "array_num : " << array_num << " rand_max : " << rand_max << " rand_min : " << rand_min << " type : " << "select2";
		const auto t0 = high_resolution_clock::now();

		Ans = make_nonrepeat_rand_array_select2(array_num, rand_min, rand_max);

		const auto t1 = high_resolution_clock::now();
		const auto time = duration_cast<milliseconds>(t1 - t0);

		std::cout << " = " << time.count() << "msec" << std::endl;
	}
	{
		int* Ans;

		std::cout << "array_num : " << array_num << " rand_max : " << rand_max << " rand_min : " << rand_min << " type : " << "fn3_2_3_select";
		const auto t0 = high_resolution_clock::now();
		
		Ans = fn3_2_3_make_rand_array_select(array_num, rand_min, rand_max);

		const auto t1 = high_resolution_clock::now();
		const auto time = duration_cast<milliseconds>(t1 - t0);
		std::cout << " = " << time.count() << "msec" << std::endl;


		free(Ans);
	}



	system("pause");
	return 0;
}
