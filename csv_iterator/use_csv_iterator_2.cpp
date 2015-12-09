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

	//1行読み飛ばす
	csv::advance_line(itr);

	//名前、性別、年齢の順で読みだす
	while(itr != csv::end(fin)){
		//文字列なら直接読み出し可能
		Names.push_back(*itr++);
		if(itr.eol())continue;

		//当然、文字列との比較も可能
		IsFemales.push_back(*itr++ == "F");
		if(itr.eol())continue;

		//型を指定して読み出すことも可能
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
