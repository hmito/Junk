// [[Rcpp::plugins(cpp20)]]   
#include <Rcpp.h>
#include"hmLib/numeric.hpp"
using namespace Rcpp;

// [[Rcpp::export]]
Rcpp::List NEtest(double x0, double y0, double relval=0.2,double absval = 0.001, double a=1, double b=100) {
	std::vector<double> Ini{x0,y0};
	auto ans = hmLib::numeric::nelder_mead_minima([=](const std::vector<double>& vec){return (a-vec[0])*(a-vec[0]) + b*(vec[1]-vec[0]*vec[0])*(vec[1]-vec[0]*vec[0]);}, Ini, relval, absval,500000,1e-8,1e-8);
	
	if(ans.success()){
		return List::create(Named("success")=ans.success(),Named("count")=ans.count, Named("value")=ans->e, Named("x")=ans->v[0], Named("y")=ans->v[1]);
	}else{
		return List::create(Named("success")=ans.success(),Named("count")=ans.count);		
	}
}
