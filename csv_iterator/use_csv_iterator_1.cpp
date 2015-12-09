#include <iostream>
#include <fstream>
#include <string>
#include "csv_iterator.hpp"

int main(){
	auto oitr = csv::current(std::cout, csv::mode::tsv);

	std::vector<std::string> Names;
	std::vector<bool> IsFemales;
	std::vector<int> Ages;

	std::ifstream fin("data.csv");
	auto itr = csv::begin(fin, csv::mode::csv);
	csv::
	for(; itr != csv::end(fin); ++itr){
		*oitr++ = *itr;
		if(itr.eol())oitr.endl();
	}

	system("pause");

	return 0;
}
