#include <iostream>
#include <fstream>
#include "csv_iterator.hpp"

int main(){
	std::ifstream fin("data2.csv");
	auto itr = csv::begin(fin, csv::mode::csv);

	//1�s�ǂݔ�΂�
	csv::advance_line(itr);

	//���O�A���ʁA�N��̏��œǂ݂���
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
