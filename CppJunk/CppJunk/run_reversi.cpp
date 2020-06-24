#include"reversi.hpp"

int main() {
	{
		auto p1 = std::make_unique<reversi::WEC_AI>(10.0, 0.10, 5.0, 0.20, 0.50);
		auto p2 = std::make_unique<reversi::MINO_AI>(10.0, 0.10, 5.0, 0.20, 0.50);
		reversi::bw_table Table(std::move(p1), std::move(p2));

		int p1win = 0;

		for (unsigned int i = 0; i < 500; ++i) {
			Table.reset();
			for (unsigned int i = 0; i < 60; ++i) {
				auto a = Table.step();
			}
			unsigned int score_b = std::count(Table.get().begin(), Table.get().end(), reversi::cell::black);
			unsigned int score_w = std::count(Table.get().begin(), Table.get().end(), reversi::cell::white);

			if (score_b > score_w)++p1win;
		}

		std::cout << p1win << " : " << 1000 - p1win << std::endl;
	}
	{
		auto p1 = std::make_unique<reversi::MINO_AI>(10.0, 0.10, 5.0, 0.20, 0.50);
		auto p2 = std::make_unique<reversi::WEC_AI>(10.0, 0.10, 5.0, 0.20, 0.50);
		reversi::bw_table Table(std::move(p1), std::move(p2));

		int p1win = 0;

		for (unsigned int i = 0; i < 500; ++i) {
			Table.reset();
			for (unsigned int i = 0; i < 60; ++i) {
				auto a = Table.step();
			}
			unsigned int score_b = std::count(Table.get().begin(), Table.get().end(), reversi::cell::black);
			unsigned int score_w = std::count(Table.get().begin(), Table.get().end(), reversi::cell::white);

			if (score_b > score_w)++p1win;
		}

		std::cout << p1win << " : " << 1000 - p1win << std::endl;
	}

	/*
	unsigned int prev_w = 2;
	unsigned int prev_b = 2;
	for (unsigned int i = 0; i < 60; ++i) {
		auto a = Table.step();
		std::cout << Table.get();

		unsigned int score_b = std::count(Table.get().begin(), Table.get().end(), reversi::cell::black);
		unsigned int score_w = std::count(Table.get().begin(), Table.get().end(), reversi::cell::white);
		if (a.first.c == reversi::cell::black) {
			std::cout << " B :[W]= ";
		} else {
			std::cout << "[B]: W = ";
		}
		std::cout << score_b
			<< " : "
			<< score_w
			<< std::endl;

		if (std::count(Table.get().begin(), Table.get().end(), reversi::cell::none) == 0)break;

		if (i > 0 && i % 2==0) {
			if (prev_w == score_w && prev_b == score_b)break;
			prev_w = score_w;
			prev_b = score_b;
		}
	}
	*/
	return 0;
}