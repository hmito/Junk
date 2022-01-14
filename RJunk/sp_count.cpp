#include <random>
#include <numeric>
#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
IntegerVector countSp(IntegerVector x) {
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());

	IntegerVector y(x.begin(), x.end());
	IntegerVector SpNums(std::accumulate(y.begin(), y.end(), 0));
	
	int SpNum = 0;
	for (int i = 0; i < SpNums.size(); ++i) {
		//ランダムに未発見の個体をサンプル
		int spno = std::discrete_distribution<int>(y.begin(), y.end())(engine);
		if (x[spno] == y[spno]) {
			++SpNum;
			//全種を見つけたら、残りも総種数で埋めて終了
			if(SpNum==x.size()){
				for(int j = i; j < SpNums.size(); ++j){
					SpNums[j] = SpNum;
				}
				break;
			}
		}
		//発見された個体を取り除く
		--y[spno];
		//i+1個体捕まえたときの種数を記録
		SpNums[i] = SpNum;
	}
	return SpNums;
}
