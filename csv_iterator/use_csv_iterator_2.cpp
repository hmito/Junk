#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "csv_iterator.hpp"

int main(){
	auto oitr = csv::current(std::cout, csv::mode::tsv);

	std::vector<std::string> Names;
	std::vector<bool> IsFemales;
	std::vector<int> Ages;

	std::ifstream fin("data.csv");
	auto itr = csv::begin(fin, csv::mode::csv);

	//1�s�ǂݔ�΂�
	csv::advance_line(itr);

	//���O�A���ʁA�N��̏��œǂ݂���
	while(itr != csv::end(fin)){
		//������Ȃ璼�ړǂݏo���\
		Names.push_back(*itr++);
		if(itr.eol())continue;

		//���R�A������Ƃ̔�r���\
		IsFemales.push_back(*itr++ == "F");
		if(itr.eol())continue;

		//�^���w�肵�ēǂݏo�����Ƃ��\
		Ages.push_back((*itr++).read<int>());
	}

	std::cout << "=name=" << std::endl;
	for(const auto& val : Names)std::cout << val << std::endl;
	std::cout << std::endl;

	std::cout << "=sex=" << std::endl;
	for(auto val : IsFemales)std::cout << val << std::endl;
	std::cout << std::endl;

	std::cout << "=age=" << std::endl;
	for(auto val : Ages)std::cout << val << std::endl;
	std::cout << std::endl;

	system("pause");

	return 0;
}
