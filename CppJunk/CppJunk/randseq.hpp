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
namespace detail {
	template<typename T> auto diff(T n1, T n2) -> typename std::make_unsigned<T>::type {
		static_assert(std::is_integral<T>::value, "T is not integral.");
		if (n1 < n2) std::swap(n1, n2);
		return static_cast<typename std::make_unsigned<T>::type>(n1 - n2);
	}
}
std::vector<int> make_rand_array_hash(const size_t size, int rand_min, int rand_max){
	using type = decltype(rand_min);
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if(max_min_diff < size) throw std::runtime_error("Invalid argument");

	std::unordered_set<type> tmp;
	auto engine = create_rand_engine();
	std::uniform_int_distribution<type> distribution(rand_min, rand_max);
	while(tmp.size() < size) tmp.insert(distribution(engine));
	return std::vector<type>(tmp.begin(), tmp.end());
}

std::vector<int> make_rand_array_unique(const size_t size, int rand_min, int rand_max){
	using type = decltype(rand_min);
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if(max_min_diff < size) throw std::runtime_error("Invalid argument");

	std::vector<type> tmp;
	auto engine = create_rand_engine();
	std::uniform_int_distribution<type> distribution(rand_min, rand_max);

	const size_t make_size = (static_cast<uintmax_t>(std::numeric_limits<double>::max()) < size) ? ((std::numeric_limits<size_t>::max() - size / 5) < size) ? size : size + size / 5 : static_cast<size_t>(size*1.2);
	tmp.reserve(make_size);
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
	return tmp;
}

std::vector<int> make_rand_array_shuffle(const size_t size, int rand_min, int rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if(max_min_diff < size) throw std::runtime_error("Invalid argument");

	std::vector<decltype(rand_min)> tmp;
	tmp.reserve(max_min_diff);

	for(auto i = rand_min; i <= rand_max; ++i) tmp.push_back(i);

	auto engine = create_rand_engine();
	std::shuffle(tmp.begin(), tmp.end(), engine);

	tmp.erase(std::next(tmp.begin(), size), tmp.end());

	return tmp;
}
std::vector<int> make_rand_array_select(const size_t size, int rand_min, int rand_max) {
	using type = decltype(rand_min);
	if (rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if (max_min_diff < size) throw std::runtime_error("Invalid argument");

	std::vector<type> tmp;
	tmp.reserve(max_min_diff);

	for (auto i = rand_min; i <= rand_max; ++i)tmp.push_back(i);

	auto engine = create_rand_engine();
	std::uniform_int_distribution<type> distribution(rand_min, rand_max);

	for (size_t cnt = 0; cnt < size; ++cnt) {
		size_t pos = std::uniform_int_distribution<size_t>(cnt, tmp.size() - 1)(engine);

		if (cnt != pos) std::swap(tmp[cnt], tmp[pos]);
	}
	tmp.erase(std::next(tmp.begin(), size), tmp.end());

	return tmp;
}

std::vector<int> make_rand_array_just_shuffle(const size_t size, int rand_min, int rand_max) {
	using type = decltype(rand_min);
	if (rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if (max_min_diff != size) throw std::runtime_error("Invalid argument");

	auto engine = create_rand_engine();
	std::uniform_int_distribution<type> distribution(rand_min, rand_max);
	std::vector<type> re(size);
	auto t = rand_min;
	std::generate(re.begin(), re.end(), [&t]() { return t++; });
	std::shuffle(re.begin(), re.end(), engine);
	return re;
}
std::vector<int> make_rand_array(const size_t size, int rand_min, int rand_max) {
	if (rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if (max_min_diff < size) throw std::runtime_error("Invalid argument");
	
	if (max_min_diff == size) return make_rand_array_just_shuffle(size, rand_min, rand_max);
	else if (static_cast<uintmax_t>(std::numeric_limits<double>::max()) < max_min_diff || size < (max_min_diff * 0.04)) {
		return make_rand_array_unique(size, rand_min, rand_max);
	}
	else {
		return make_rand_array_select(size, rand_min, rand_max);
	}
}
