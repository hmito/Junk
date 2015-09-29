#pragma once
#
#include<chrono>
#include<vector>
#include<array>
#include<utility>
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

		if(size < std::distance(tmp.begin(), unique_end)){
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

std::vector<int> make_rand_array_just_shuffle(const size_t size, int rand_min, int rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const size_t max_min_diff = static_cast<size_t>(rand_max - rand_min + 1);
	if(max_min_diff < size) throw std::runtime_error("引数が異常です");

	std::vector<int> tmp;
	tmp.reserve(max_min_diff);

	for(int i = rand_min; i <= rand_max; ++i)tmp.push_back(i);

	auto engine = create_rand_engine();
	std::shuffle(tmp.begin(), tmp.end(), engine);

	return std::move(tmp);
}

std::vector<int> make_rand_array_select(const size_t size, int rand_min, int rand_max){
	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const size_t max_min_diff = static_cast<size_t>(rand_max - rand_min + 1);
	if(max_min_diff < size) throw std::runtime_error("引数が異常です");

	std::vector<int> tmp;
	tmp.reserve(max_min_diff);

	for(int i = rand_min; i <= rand_max; ++i)tmp.push_back(i);

	auto engine = create_rand_engine();
	std::uniform_int_distribution<int> distribution(rand_min, rand_max);

	for(size_t cnt = 0; cnt < size; ++cnt){
		size_t pos =std::uniform_int_distribution<size_t>(cnt, tmp.size()-1)(engine);

		if(cnt != pos) std::swap(tmp[cnt], tmp[pos]);
	}
	tmp.erase(std::next(tmp.begin(), size), tmp.end());

	return std::move(tmp);
}
