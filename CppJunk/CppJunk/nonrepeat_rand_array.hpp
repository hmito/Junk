#pragma once

#include <random>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>
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

template<typename type>
std::vector<type> make_nonrepeat_rand_array_hash(const size_t size, type rand_min, type rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if(max_min_diff < size) throw std::runtime_error("Invalid argument");

	std::unordered_set<type> tmp;
	auto engine = create_rand_engine();
	std::uniform_int_distribution<type> distribution(rand_min, rand_max);
	while(tmp.size() < size) tmp.insert(distribution(engine));
	return std::vector<type>(tmp.begin(), tmp.end());
}

template<typename type>
std::vector<type> make_nonrepeat_rand_array_unique(const size_t size, type rand_min, type rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if(max_min_diff < size) throw std::runtime_error("Invalid argument");

	std::vector<type> tmp;
	auto engine = create_rand_engine();
	std::uniform_int_distribution<type> distribution(rand_min, rand_max);

	//ひとまず、size/5だけ多めに作ってから、重複を消す。
	//この数字に根拠はないので、より最適な値がある可能性あり
	const size_t make_size = (std::numeric_limits<size_t>::max() - size / 5) < size? size: size + size / 5;

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

template<typename type>
std::vector<type> make_nonrepeat_rand_array_shuffle(const size_t size, type rand_min, type rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if(max_min_diff < size) throw std::runtime_error("Invalid argument");

	std::vector<type> tmp;
	tmp.reserve(max_min_diff);

	for(auto i = rand_min; i <= rand_max; ++i)tmp.push_back(i);

	auto engine = create_rand_engine();
	std::shuffle(tmp.begin(), tmp.end(), engine);

	tmp.erase(std::next(tmp.begin(), size), tmp.end());

	return tmp;
}

template<typename type>
std::vector<type> make_nonrepeat_rand_array_select(const size_t size, type rand_min, type rand_max) {
	if (rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if (max_min_diff < size) throw std::runtime_error("Invalid argument");

	std::vector<type> tmp;
	tmp.reserve(max_min_diff);

	for (auto i = rand_min; i <= rand_max; ++i)tmp.push_back(i);

	auto engine = create_rand_engine();

	for (size_t cnt = 0; cnt < size; ++cnt) {
		size_t pos = std::uniform_int_distribution<size_t>(cnt, tmp.size() - 1)(engine);

		if (cnt != pos) std::swap(tmp[cnt], tmp[pos]);
	}
	tmp.erase(std::next(tmp.begin(), size), tmp.end());

	return tmp;
}

template<typename type>
std::vector<type> make_nonrepeat_rand_array_select_with_hash(const size_t size, type rand_min, type rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if(max_min_diff < size) throw std::runtime_error("Invalid argument");
	using hash_map = std::unordered_map<type, type>;
	
	std::vector<type> tmp;
	tmp.reserve(size);

	hash_map Map;

	auto engine = create_rand_engine();
	for(size_t cnt = 0; cnt < size; ++cnt){
		type val = std::uniform_int_distribution<type>(rand_min, rand_max)(engine);
		auto itr = Map.find(val);

		size_t replaced_val;
		auto replaced_itr = Map.find(rand_max);
		if(replaced_itr !=Map.end()) replaced_val = replaced_itr->second;
		else replaced_val = rand_max;

		if(itr == Map.end()){
			tmp.push_back(val);
			if(val!=rand_max)Map.insert(std::make_pair(val, replaced_val));
		} else{
			tmp.push_back(itr->second);
			itr->second = replaced_val;
		}

		//Map.erase(replaced_val);
		
		--rand_max;
	}

	return tmp;
}

template<typename type>
std::vector<type> make_nonrepeat_rand_array_select_with_hash_no_itr(const size_t size, type rand_min, type rand_max) {
	if (rand_min > rand_max) std::swap(rand_min, rand_max);
	auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if (max_min_diff < size) throw std::runtime_error("引数が異常です");
	std::vector<type> re(size);
	std::unordered_map<type, type> conversion;

	auto engine = create_rand_engine();
	for (auto& r : re) {
		type key = std::uniform_int_distribution<type>(0, max_min_diff)(engine);
		if (!conversion.count(key)) conversion[key] = key;
		auto& conv_at_key = conversion[key];
		r = conv_at_key + rand_min;
		type conv_key = static_cast<type>(max_min_diff - 1);
		conv_at_key = (conversion.count(conv_key)) ? conversion[conv_key] : conv_key;
		--max_min_diff;
	}

	return re;
}


template<typename type>
std::vector<type> make_nonrepeat_rand_array(const size_t size, type rand_min, type rand_max) {
	if (rand_min > rand_max) std::swap(rand_min, rand_max);
	const auto max_min_diff = detail::diff(rand_max, rand_min) + 1;
	if (max_min_diff < size) throw std::runtime_error("Invalid argument");
	
	if (size < max_min_diff/32) {
		return make_nonrepeat_rand_array_unique(size, rand_min, rand_max);
	}
	else {
		return make_nonrepeat_rand_array_select(size, rand_min, rand_max);
	}
}
