#include <random>
#include <numeric>
#include <vector>

using IntegerVector = std::vector<int>;
// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//

// [[Rcpp::export]]
IntegerVector countSp(IntegerVector x) {
	static std::random_device seed_gen;
	std::mt19937 engine(seed_gen());

	IntegerVector y(x.begin(), x.end());
	IntegerVector Ans(std::accumulate(y.begin(), y.end(), 0));

	int SpNum = 0;
	for (int i = 0; i < Ans.size(); ++i) {
		int spno = std::discrete_distribution<int>(y.begin(), y.end())(engine);
		if (x[spno] == y[spno]) {
			++SpNum;
		}
		--y[spno];
		Ans[i] = SpNum;
	}
	return Ans;
}

int main() {
	IntegerVector Data{2,5,6,3,4,5};

	auto Ans = countSp(Data);

	return 0;
}