#pragma once

#include <random>
#include <vector>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <functional>
#include <limits>
#include <type_traits>

std::mt19937 create_rand_engine(){
	std::random_device rnd;
	std::vector<std::uint_least32_t> v(10);// 初期化用ベクタ
	std::generate(v.begin(), v.end(), std::ref(rnd));// ベクタの初期化
	std::seed_seq seed(v.begin(), v.end());
	return std::mt19937(seed);// 乱数エンジン
}

std::vector<int> make_rand_array_hash(const size_t size, int rand_min, int rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const size_t max_min_diff = static_cast<size_t>(rand_max - rand_min + 1);
	if(max_min_diff < size) throw std::runtime_error("引数が異常です");

	std::unordered_set<int> tmp;
	auto engine = create_rand_engine();
	std::uniform_int_distribution<int> distribution(rand_min, rand_max);
	while(tmp.size() < size) tmp.insert(distribution(engine));
	return std::vector<int>(tmp.begin(), tmp.end());
}

std::vector<int> make_rand_array_unique(const size_t size, int rand_min, int rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const size_t max_min_diff = static_cast<size_t>(rand_max - rand_min + 1);
	if(max_min_diff < size) throw std::runtime_error("引数が異常です");

	std::vector<int> tmp;
	auto engine = create_rand_engine();
	std::uniform_int_distribution<int> distribution(rand_min, rand_max);

	const size_t make_size = static_cast<size_t>(size*1.2);

	while(tmp.size() < size){
		while(tmp.size() < make_size) tmp.push_back(distribution(engine));
		std::sort(tmp.begin(), tmp.end());
		auto unique_end = std::unique(tmp.begin(), tmp.end());

		if(size < static_cast<size_t>(std::distance(tmp.begin(), unique_end))){
			unique_end = std::next(tmp.begin(), size);
		}
		tmp.erase(unique_end, tmp.end());
	}

	std::shuffle(tmp.begin(), tmp.end(), engine);
	return std::move(tmp);
}

std::vector<int> make_rand_array_shuffle(const size_t size, int rand_min, int rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const size_t max_min_diff = static_cast<size_t>(rand_max - rand_min + 1);
	if(max_min_diff < size) throw std::runtime_error("引数が異常です");

	std::vector<int> tmp;
	tmp.reserve(max_min_diff);

	for(int i = rand_min; i <= rand_max; ++i)tmp.push_back(i);

	auto engine = create_rand_engine();
	std::shuffle(tmp.begin(), tmp.end(), engine);

	tmp.erase(std::next(tmp.begin(), size), tmp.end());

	return std::move(tmp);
}
namespace detail {
	template<typename Re>class affluence_inc_gen_c {
	public:
		affluence_inc_gen_c(uintmax_t affluence) : affluence_(affluence) {}
		Re operator()(std::mt19937& engin) {
			const auto re = std::uniform_int_distribution<Re>(0, this->make_rand_max())(engin);
			this->affluence_ -= re;
			return re;
		}
	private:
		Re make_rand_max() { return (this->affluence_ < std::numeric_limits<Re>::max()) ? static_cast<Re>(this->affluence_) : std::numeric_limits<Re>::max(); }
		uintmax_t affluence_;
	};
	template<typename T> auto diff(T n1, T n2) -> typename std::make_unsigned<T>::type {
		static_assert(std::is_integral<T>::value, "T is not integral.");
		if (n1 < n2) std::swap(n1, n2);
		return static_cast<typename std::make_unsigned<T>::type>(n1 - n2);
	}
}
std::vector<int> make_rand_array_liner_gen_shuffle(const size_t size, int rand_min, int rand_max) {
	using type = decltype(rand_min);
	if (rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if (max_min_diff < size) throw std::runtime_error("引数が異常です");
	const auto affluence = max_min_diff - size;
	auto engine = create_rand_engine();
	if (0 == affluence) return [size, rand_min, &engine]() {
		std::vector<type> re(size);
		int t = rand_min;
		for (auto& i : re) i = t++;
		std::shuffle(re.begin(), re.end(), engine);
		return re;
	}();
	std::vector<type> re;
	re.reserve(size);
	detail::affluence_inc_gen_c<type> affluence_inc(affluence);
	for (auto i = affluence_inc(engine) + rand_min; re.size() < size; i += affluence_inc(engine) + 1) re.push_back(i);
	std::shuffle(re.begin(), re.end(), engine);
	return std::move(re);
}
