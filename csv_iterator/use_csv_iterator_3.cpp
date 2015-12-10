#include <iostream>
#include <fstream>
#include "csv_iterator.hpp"

int main(){
	std::ifstream fin("data2.csv");
	auto itr = csv::begin(fin, csv::mode::csv);

	//1行読み飛ばす
	csv::advance_line(itr);

	//名前、性別、年齢の順で読みだす
	while(itr != csv::end(fin)){
		unsigned int Num;
		(*itr++) >> Num;

		for(unsigned int Cnt = 0; Cnt < Num; ++Cnt){
			std::cout << "[" << Cnt << "/" << Num << "]" << (*itr++) <<std::endl;
		}
	}

	system("pause");

	return 0;
}
