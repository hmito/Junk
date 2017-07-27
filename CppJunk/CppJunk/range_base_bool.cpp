#include<vector>
#include<iostream>

int main(void) {
	std::vector<bool> Vec{ true,false,false,false,true,false,true };
//	std::vector<int> Vec{ 1,0,0,0,1,0,1};

	for (bool v : Vec) {
		if (v) std::cout << "t" << ",";
		else std::cout << "f" << ",";
		v = !v;
	}
	std::cout << std::endl;

/*	for (bool& v : Vec) {
		if (v)std::cout << "t" << ",";
		else std::cout << "f" << ",";
	}
	std::cout << std::endl;
*/

	for (const bool& v : Vec) {
		if (v)std::cout << "t" << ",";
		else std::cout << "f" << ",";
		//v = !v;
	}
	std::cout << std::endl;


	for (auto v : Vec) {
		if (v)std::cout << "t" << ",";
		else std::cout << "f" << ",";
		v = !v;
	}
	auto v1 = *(Vec.begin());
	std::cout << typeid(v1).name() << std::endl;

/*	for (auto& v : Vec) {
		if (v)std::cout << "t" << ",";
		else std::cout << "f" << ",";
	}
	std::cout << std::endl;
*/
	for (const auto& v : Vec) {
		if (v)std::cout << "t" << ",";
		else std::cout << "f" << ",";
		//v = !v;
	}
	const auto& v = *(Vec.begin());
	std::cout << typeid(v).name()<< std::endl;

	for (auto&& v : Vec) {
		if (v)std::cout << "t" << ",";
		else std::cout << "f" << ",";
		v = !v;
	}
	auto&& v2 = *(Vec.begin());
	std::cout << typeid(v2).name() << std::endl;

	system("pause");
	return 0;
}