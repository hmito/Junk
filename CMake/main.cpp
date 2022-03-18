#include<iostream>

int main(){
	std::cout<< "Hello world of C++ with CMake." <<std::endl;
	int x = 0;
	int y = 1;

	x += y;

	y += x;

	x+= y;

	std::cout<<x<<std::endl;

	return 0;
}