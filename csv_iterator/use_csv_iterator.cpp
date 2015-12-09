#include <iostream>
#include <fstream>
#include <string>
#include "csv_iterator.hpp"

int main(){
	auto oitr = csv::current(std::cout, csv::mode::tsv);

	std::ifstream fin("data.csv");
	for(auto itr = csv::begin(fin, csv::mode::csv); itr != csv::end(fin); ++itr){
		*oitr++ = *itr;
		if(itr.eol())oitr.endl();
	}

	system("pause");

	return 0;
}
