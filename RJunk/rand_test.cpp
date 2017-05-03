//[[Rcpp::plugins(cpp11)]]
//[[Rcpp::depends(BH)]]

#define HMLIB_CONFIG_INC

#include <Rcpp.h>
#include "../hmLib/hmLib/random.hpp"

// [[Rcpp::export]]
Rcpp::List create_pair_normal(double e1, double e2, double sigma1, double sigma2, double cov, unsigned int Num){
	hmLib::pair_normal_distribution<double> Dist(e1, e2, sigma1, sigma2, cov);
	hmLib::random::default_engine Engine;
	
	Rcpp::NumericVector V1(Num,0.0);
	Rcpp::NumericVector V2(Num,0.0);	
	
	for(unsigned int Cnt = 0; Cnt<Num; ++Cnt){
		auto val = Dist(Engine);
		V1[Cnt] = val.first;
		V2[Cnt] = val.second;
	}
	
	return Rcpp::List::create(
		Rcpp::Named("v1") = V1, Rcpp::Named("v2") = V2
	);
}

// [[Rcpp::export]]
Rcpp::List create_pair_normal_xos(double e1, double e2, double sigma1, double sigma2, double cov, unsigned int Num){
	hmLib::pair_normal_distribution<double> Dist(e1, e2, sigma1, sigma2, cov);
	auto Engine = hmLib::random::randomized_engine<hmLib::xorshift128>();
	
	Rcpp::NumericVector V1(Num,0.0);
	Rcpp::NumericVector V2(Num,0.0);	

	for(unsigned int Cnt = 0; Cnt<Num; ++Cnt){
		auto val = Dist(Engine);
		V1[Cnt] = val.first;
		V2[Cnt] = val.second;
	}
	
	return Rcpp::List::create(
		Rcpp::Named("v1") = V1, Rcpp::Named("v2") = V2
	);
}
