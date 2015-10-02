#ifndef NONREPEATRANDOMINTEGRALS_INC
#define NONREPEATRANDOMINTEGRALS_INC
#
#include <random>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <unordered_map>
#include <limits>
#include <type_traits>

//Most fast method when size is much smaller than rand_max - rand_min
template<typename type,typename generator>
std::vector<type> nonrepeat_random_integrals_by_SortAndUnique(const std::size_t size, type rand_min, type rand_max, generator& Engine){
	using unsigned_type = std::make_unsigned<type>::type;

	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const unsigned_type max_min_diff = rand_max - rand_min + 1;
	if(max_min_diff<size)throw std::runtime_error("Integrals in [rand_min, rand_max] is fewer than requested size.");

	std::vector<type> tmp;
	std::uniform_int_distribution<type> distribution(rand_min, rand_max);

	const std::size_t make_size = (std::numeric_limits<std::size_t>::max() - size / 5) < size ? size : size + size / 5;

	tmp.reserve(make_size);
	while(tmp.size() < size){
		while(tmp.size() < make_size) tmp.push_back(distribution(Engine));
		std::sort(tmp.begin(), tmp.end());
		auto unique_end = std::unique(tmp.begin(), tmp.end());

		if(size < static_cast<std::size_t>(std::distance(tmp.begin(), unique_end))){
			unique_end = std::next(tmp.begin(), size);
		}
		tmp.erase(unique_end, tmp.end());
	}

	std::shuffle(tmp.begin(), tmp.end(), Engine);
	return tmp;
}

//Most fast method when size is little smaller than rand_max - rand_min
template<typename type, typename generator>
std::vector<type> nonrepeat_random_integrals_by_FisherYates(const std::size_t size, type rand_min, type rand_max, generator& Engine){
	using unsigned_type = std::make_unsigned<type>::type;

	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const unsigned_type max_min_diff = rand_max - rand_min + 1;
	if(max_min_diff<size)throw std::runtime_error("Integrals in [rand_min, rand_max] is fewer than requested size.");

	std::vector<type> tmp;
	tmp.assign(max_min_diff,0);
	auto val = rand_min;
	std::generate(tmp.begin(), tmp.end(), [&val](){return val++; });

	std::uniform_int_distribution<type> distribution(rand_min, rand_max);

	for(std::size_t cnt = 0; cnt < size; ++cnt){
		std::size_t pos = std::uniform_int_distribution<std::size_t>(cnt, tmp.size() - 1)(Engine);

		if(cnt != pos) std::swap(tmp[cnt], tmp[pos]);
	}
	tmp.erase(std::next(tmp.begin(), size), tmp.end());

	return tmp;
}

//Combination method of SortAndUnique and FisherYates.
//	This is (maybe) the fastest function for generating nonrepeat random integrals in most cases.
template<typename type, typename generator>
std::vector<type> nonrepeat_random_integrals(const std::size_t size, type rand_min, type rand_max, generator& Engine){
	using unsigned_type = std::make_unsigned<type>::type;

	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const unsigned_type max_min_diff = rand_max - rand_min + 1;

	if(size < max_min_diff / 33){
		return nonrepeat_random_integrals_by_SortAndUnique(size, rand_min, rand_max, Engine);
	} else{
		return nonrepeat_random_integrals_by_FisherYates(size, rand_min, rand_max, Engine);
	}
}

//Most balanced method for all parameter values.
//	Not so fast, but the performancs is stable for any parameter values.
template<typename type>
std::vector<type> nonrepeat_random_integrals_by_FisherYatesWithHash(const size_t size, type rand_min, type rand_max){
	using unsigned_type = std::make_unsigned<type>::type;

	if(rand_min > rand_max) std::swap(rand_min, rand_max);
	const unsigned_type max_min_diff = rand_max - rand_min + 1;
	if(max_min_diff<size)throw std::runtime_error("Integrals in [rand_min, rand_max] is fewer than requested size.");

	using hash_map = std::unordered_map<type, type>;

	std::vector<type> tmp;
	tmp.reserve(size);

	hash_map Map;

	auto engine = create_rand_engine();
	for(size_t cnt = 0; cnt < size; ++cnt){
		size_t val = std::uniform_int_distribution<size_t>(rand_min, rand_max)(engine);
		hash_map::iterator itr = Map.find(val);

		size_t replaced_val;
		hash_map::iterator replaced_itr = Map.find(rand_max);
		if(replaced_itr != Map.end()) replaced_val = replaced_itr->second;
		else replaced_val = rand_max;

		if(itr == Map.end()){
			tmp.push_back(val);
			if(val != rand_max)Map.insert(std::make_pair(val, replaced_val));
		} else{
			tmp.push_back(itr->second);
			itr->second = replaced_val;
		}

		--rand_max;
	}

	return tmp;
}
#
#endif
