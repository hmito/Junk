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

	constexpr unsigned int size = 100000;
	int rand_min = 0;
	int rand_max = 10000000;

	std::cout << "size : " << size << ", rand_max : " << rand_max << ", rand_min : " << rand_min << ", and_max / size : " << (rand_max- rand_min)/size<<std::endl;

	{
		std::vector<int> Ans;
		std::cout << "select";
		const auto t0 = high_resolution_clock::now();

		Ans = make_nonrepeat_rand_array_select(size, rand_min, rand_max);

		const auto t1 = high_resolution_clock::now();
		const auto time = duration_cast<milliseconds>(t1 - t0);

		std::cout << " = " << time.count() << "msec" << std::endl;
	}

	{
		std::vector<int> Ans;
		std::cout << "select2";
		const auto t0 = high_resolution_clock::now();

		Ans = make_nonrepeat_rand_array_select2(size, rand_min, rand_max);

		const auto t1 = high_resolution_clock::now();
		const auto time = duration_cast<milliseconds>(t1 - t0);

		std::cout << " = " << time.count() << "msec" << std::endl;
	}
	{
		int* Ans;

		std::cout << "fn3_2_select";
		const auto t0 = high_resolution_clock::now();
		
		Ans = fn3_2_make_rand_array_select(size, rand_min, rand_max);

		const auto t1 = high_resolution_clock::now();
		const auto time = duration_cast<milliseconds>(t1 - t0);
		std::cout << " = " << time.count() << "msec" << std::endl;


		free(Ans);
	}



	system("pause");
	return 0;
}
