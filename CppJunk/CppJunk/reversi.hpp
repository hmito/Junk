#ifndef REVERSI_INC
#define REVERSI_INC 100
#include<vector>
#include<algorithm>
#include<random>
#include<numeric>
#include<iostream>
namespace reversi {
	enum class cell :char {
		out = -1,
		none = 0,
		black = 1,
		white = 2
	};
	struct point {
		unsigned int x;
		unsigned int y;
	};
	struct hand {
		cell c;
		point p;
	};

	struct board {
		using container = std::vector<cell>;
		using iterator = container::iterator;
		using const_iterator = container::const_iterator;
	private:
		container data;
	public:
		board() :data(64, cell::none) {}
		void reset() {
			std::fill(data.begin(), data.end(), cell::none);
		}
		cell get(point p)const {
			if (p.x >= 8) return cell::out;
			if (p.y >= 8) return cell::out;
			return data[p.x + p.y * 8u];
		}
		bool set(cell c, point p) {
			if (p.x >= 8) return true;
			if (p.y >= 8) return true;
			data[p.x + p.y * 8u] = c;
			return false;
		}
		unsigned int eval(hand h)const {
			if (get(h.p) != cell::none)return 0;

			//check eight direction for loop
			unsigned int ReversedCnt = 0;
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					if (dx == 0 && dy == 0)continue;

					cell endc = cell::none;
					unsigned int rlen = 1;
					for (; rlen <= 10; ++rlen) {
						endc = get(point{ h.p.x + dx * rlen, h.p.y + dy * rlen });

						//end of reversible cells
						if (endc == cell::out || endc == cell::none || endc == h.c)break;
					}

					//success for finding reversible cells
					if (endc == h.c && rlen > 1) {
						ReversedCnt += rlen - 1;
					}
				}
			}

			//fail to find any reversible cell.
			if (ReversedCnt == 0)return 0;

			//add own cell
			return ReversedCnt + 1;
		}
		bool put(hand h) {
			if (get(h.p) != cell::none)return true;

			//check eight direction for loop
			bool Reversed = false;
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					if (dx == 0 && dy == 0)continue;

					cell endc = cell::none;
					unsigned int rlen = 1;
					for (; rlen <= 10; ++rlen) {
						endc = get(point{ h.p.x + dx * rlen, h.p.y + dy * rlen });

						//end of reversible cells
						if (endc == cell::out || endc == cell::none || endc == h.c)break;
					}

					//success for finding reversible cells
					if (endc == h.c && rlen > 1) {
						Reversed = true;
						for (unsigned int i = 1; i < rlen; ++i) {
							set(h.c, point{ h.p.x + dx * i, h.p.y + dy * i });
						}
					}
				}
			}

			//fail to find any reversible cell.
			if (!Reversed)return true;

			set(h.c, h.p);
			return false;
		}
		iterator begin() { return data.begin(); }
		iterator end() { return data.end(); }
		const_iterator begin()const { return data.begin(); }
		const_iterator end()const { return data.end(); }
	};

	class player_interface {
	public:
		virtual point operator()(cell c, const board& b) = 0;
	};
	struct bw_table {
	private:
		board brd;
		std::unique_ptr<player_interface> p1;
		std::unique_ptr<player_interface> p2;
		unsigned char op;
	public:
		bw_table(std::unique_ptr<player_interface> p1_, std::unique_ptr<player_interface> p2_)
			: brd()
			, p1(std::move(p1_))
			, p2(std::move(p2_))
			, op(0){
			reset();
		}
		void reset() {
			brd.reset();
			brd.set(reversi::cell::black, point{ 3,3 });
			brd.set(reversi::cell::black, point{ 4,4 });
			brd.set(reversi::cell::white, point{ 3,4 });
			brd.set(reversi::cell::white, point{ 4,3 });
		}
		const board& get()const { return brd; }
		std::pair<hand, bool> step() {
			hand h;
			if (op == 0) {
				h.c = cell::black;
				h.p = (*p1)(h.c, brd);
				op = 1;
			} else {
				h.c = cell::white;
				h.p = (*p2)(h.c, brd);
				op = 0;
			}
			return std::make_pair(h, brd.put(h));
		}
	};

	class WEC_AI : public player_interface {
	private:
		std::vector<double> Map;
		double corner_weigth;
		double acorner_weight;
		double edge_weight;
		double aedge_weight;
		double rnd_weight;
		std::mt19937 urbg;
	public:
		WEC_AI(double corner_weigth_, double acorner_weight_, double edge_weight_,double aedge_weight_, double rnd_weight_) 
			: Map(64, 0.0)
			, corner_weigth(corner_weigth_)
			, acorner_weight(acorner_weight_)
			, edge_weight(edge_weight_)
			, aedge_weight(aedge_weight_)
			, rnd_weight(rnd_weight_){
			std::vector<unsigned int> vec(10);
			std::random_device rd;
			std::generate(vec.begin(), vec.end(), std::ref(rd));
			std::seed_seq seq(vec.begin(), vec.end());
			urbg.seed(seq);
		}
		point operator()(cell c, const board& b)override {
			set_weight();
			point p;
			for (p.x = 0; p.x < 8; ++p.x) {
				for (p.y = 0; p.y < 8; ++p.y) {
					Map[p.x + 8 * p.y] *= b.eval(hand{ c, p });
				}
			}

			unsigned int dis = std::distance(Map.begin(), std::max_element(Map.begin(), Map.end()));

			return point{dis%8, dis/8};
		}
	private:
		void set_weight() {
			//set base values
			for (unsigned int ix = 0; ix < 8; ++ix) {
				for (unsigned int iy = 0; iy < 8; ++iy) {
					//x boundary
					if (ix == 0 || ix == 7) {
						//y boundary
						if (iy == 0 || iy == 7) {
							Map[ix + iy * 8] = corner_weigth;
						}//around y boundary
						else if (iy == 1 || iy == 6) {
							Map[ix + iy * 8] = acorner_weight;
						} else {
							Map[ix + iy * 8] = edge_weight;
						}
					}
					//around x boundary
					else if (ix == 1 || ix == 6) {
						//y boundary
						if (iy == 0 || iy == 7) {
							Map[ix + iy * 8] = acorner_weight;
						}//around y boundary
						else if (iy == 1 || iy == 6) {
							Map[ix + iy * 8] = acorner_weight;
						} else {
							Map[ix + iy * 8] = aedge_weight;
						}
					}
					//no boundary
					else {
						//y boundary
						if (iy == 0 || iy == 7) {
							Map[ix + iy * 8] = edge_weight;
						}//around y boundary
						else if (iy == 1 || iy == 6) {
							Map[ix + iy * 8] = aedge_weight;
						} else {
							Map[ix + iy * 8] = 1.0;
						}
					}
				}
			}
			//add random effect
			for(auto& v:Map){
				v *= (1.0 + std::uniform_real<double>(0.0, rnd_weight)(urbg));
			}
		}
	};

	class MINO_AI : public player_interface {
	private:
		std::vector<double> Map;
		double corner_weigth;
		double acorner_weight;
		double edge_weight;
		double aedge_weight;
		double rnd_weight;
		std::mt19937 urbg;
	public:
		MINO_AI(double corner_weigth_, double acorner_weight_, double edge_weight_, double aedge_weight_, double rnd_weight_)
			: Map(64, 0.0)
			, corner_weigth(corner_weigth_)
			, acorner_weight(acorner_weight_)
			, edge_weight(edge_weight_)
			, aedge_weight(aedge_weight_)
			, rnd_weight(rnd_weight_) {
			std::vector<unsigned int> vec(10);
			std::random_device rd;
			std::generate(vec.begin(), vec.end(), std::ref(rd));
			std::seed_seq seq(vec.begin(), vec.end());
			urbg.seed(seq);
		}
		point operator()(cell c, const board& b)override {
			set_weight();
			point p;
			for (p.x = 0; p.x < 8; ++p.x) {
				for (p.y = 0; p.y < 8; ++p.y) {
					auto brd = b;
					if (!brd.put(hand{ c,p })) {
						Map[p.x + 8 * p.y] /= (1 + count_open_degree(c, brd));
					} else {
						Map[p.x + 8 * p.y] = 0;
					}
				}
			}

			unsigned int dis = std::distance(Map.begin(), std::max_element(Map.begin(), Map.end()));

			return point{ dis % 8, dis / 8 };
		}
	private:
		void set_weight() {
			//set base values
			for (unsigned int ix = 0; ix < 8; ++ix) {
				for (unsigned int iy = 0; iy < 8; ++iy) {
					//x boundary
					if (ix == 0 || ix == 7) {
						//y boundary
						if (iy == 0 || iy == 7) {
							Map[ix + iy * 8] = corner_weigth;
						}//around y boundary
						else if (iy == 1 || iy == 6) {
							Map[ix + iy * 8] = acorner_weight;
						} else {
							Map[ix + iy * 8] = edge_weight;
						}
					}
					//around x boundary
					else if (ix == 1 || ix == 6) {
						//y boundary
						if (iy == 0 || iy == 7) {
							Map[ix + iy * 8] = acorner_weight;
						}//around y boundary
						else if (iy == 1 || iy == 6) {
							Map[ix + iy * 8] = acorner_weight;
						} else {
							Map[ix + iy * 8] = aedge_weight;
						}
					}
					//no boundary
					else {
						//y boundary
						if (iy == 0 || iy == 7) {
							Map[ix + iy * 8] = edge_weight;
						}//around y boundary
						else if (iy == 1 || iy == 6) {
							Map[ix + iy * 8] = aedge_weight;
						} else {
							Map[ix + iy * 8] = 1.0;
						}
					}
				}
			}
			//add random effect
			for (auto& v : Map) {
				v *= (1.0 + std::uniform_real<double>(0.0, rnd_weight)(urbg));
			}
		}
		static unsigned int count_open_degree(cell c, const board& b) {
			unsigned int opd = 0;
			for (unsigned int ix = 0; ix < 8; ++ix) {
				for (unsigned int iy = 0; iy < 8; ++iy) {
					if (b.get(point{ ix,iy }) != cell::none) {
						if (b.get(point{ ix + 1,iy }) == c
							|| b.get(point{ ix - 1,iy }) == c
							|| b.get(point{ ix ,iy + 1 }) == c
							|| b.get(point{ ix ,iy - 1 }) == c
						)++opd;
					}
				}
			}
			return opd;
		}
	};

	inline std::ostream& operator<<(std::ostream& out, const board& brd) {
		out << "  ";
		for (unsigned int i = 0; i < 8; ++i) {
			out << " " << i;
		}
		out << std::endl;

		for (unsigned int j = 0; j < 8; ++j) {
			out << " " << static_cast<char>('A' + j);
			for (unsigned int i = 0; i < 8; ++i) {
				out << " ";
				switch (brd.get(point{ i, j })) {
				case cell::none:
					out << "*";
					break;
				case cell::white:
					out << "O";
					break;
				case cell::black:
					out << "X";
					break;
				default:
					out << "!";
				}
			}
			out << std::endl;
		}

		return out;
	}

	inline void plot_hand(std::ostream& out, std::pair<hand, bool>) {

	}

}
#endif
