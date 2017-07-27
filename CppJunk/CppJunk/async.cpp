#include<future>
#include<iostream>
#include<vector>
#include<cmath>
template<typename func>
std::vector<std::pair<double,double>> solve_small_step(func&& Func, double Min, double Max, double Step) {
	std::cout << "start solve [" << Min << ":" << Max << "]" << std::endl;
	std::vector<std::pair<double, double>> Ans;

	bool WasPositive = (Func(Min)>0);
	Min += Step;

	for (; Min < Max; Min += Step) {
		bool IsPositive = (Func(Min)>0);

		if (IsPositive != WasPositive) {
			Ans.emplace_back(Min-Step, Min);
			WasPositive = IsPositive;
		}
	}
	bool IsPositive = (Func(Max)>0);

	if (IsPositive != WasPositive) {
		Ans.emplace_back(Min-Step, Max);
		WasPositive = IsPositive;
	}
	std::cout << "end solve [" << Min << ":" << Max << "]" << std::endl;

	return Ans;
}

struct func {
	double operator()(double x){
		return std::sin(x * 4 + 1.0) + 1.5*std::sin(x*1.2 - 0.4) - 2.5 * std::sin(x*3.1);
	}
};

template<typename T, typename func>
auto then(std::future<T>& Future, func&& Func){
	return std::async([Fn = std::forward<func>(Func), &Fu = Future](){Fu.wait(); return Func(Fu.get())); });
}

int main() {
	auto Solver = [](double Min, double Max) {return solve_small_step(func(), Min, Max, 1e-8); };
	auto Ans1 = std::async(Solver, 0.0, 1.0);
	auto Ans2 = std::async(Solver, 1.0, 2.0);
	auto Ans3 = std::async(Solver, 2.0, 3.0);

	Ans1.wait();
	Ans2.wait();
	Ans3.wait();

	std::vector<double> Total;
	for (auto val : Ans1.get()) {
		Total.push_back((val.first + val.second) / 2);
	}
	for (auto val : Ans2.get()) {
		Total.push_back((val.first + val.second) / 2);
	}
	for (auto val : Ans3.get()) {
		Total.push_back((val.first + val.second) / 2);
	}

	std::cout << std::endl << "=== ans ===" << std::endl;
	for (auto val : Total) {
		std::cout << val << std::endl;
	}

	system("pause");

	return 0;
}