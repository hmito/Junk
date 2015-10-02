#include <iostream>
#include <random>
#include "nonrepeat_random_integrals.hpp"

int main(void){
	std::random_device rnd;
	std::vector<std::uint_least32_t> v(10);// 初期化用ベクタ
	std::generate(v.begin(), v.end(), std::ref(rnd));// ベクタの初期化
	std::seed_seq seed(v.begin(), v.end());

	std::mt19937 Engine(seed);


	std::cout << "size:10\tmin:0\tmax:100"<<std::endl;
	auto Seq = nonrepeat_random_integrals(10, 0, 100, Engine);
	for(auto val : Seq)std::cout << val << "\t";
	std::cout << std::endl << std::endl;

	std::cout << "size:10\tmin:0\tmax:9" << std::endl;
	Seq = nonrepeat_random_integrals(10, 0, 9, Engine);
	for(auto val : Seq)std::cout << val << "\t";
	std::cout << std::endl << std::endl;

	try{
		std::cout << "size:10\tmin:0\tmax:8" << std::endl;
		Seq = nonrepeat_random_integrals(10, 0, 8, Engine);
		for(auto val : Seq)std::cout << val << "\t";
		std::cout << std::endl << std::endl;
	}
	catch(std::exception& Excp){
		std::cout << "catch exception: "<<Excp.what()<<std::endl;
	}

	system("pause");

	return 0;
}
