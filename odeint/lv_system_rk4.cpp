#include<array>
#include<string>
#include<iostream>
#include<fstream>
#include<boost/numeric/odeint.hpp>

struct lv_system{
public:
	using state = std::array<double, 2>;
public:
	double alpha;
	double beta;
	double gamma;
	double delta;
public:
	lv_system(double alpha_, double beta_, double gamma_, double delta_)
		:alpha(alpha_), beta(beta_), gamma(gamma_), delta(delta_){}

	void operator()(const state& x, state& dx, double t){
		dx[0] = x[0] * (alpha - beta* x[1]);
		dx[1] = -x[1] * (gamma - delta * x[0]);
	}
};

struct csv_observer{
	using state = lv_system::state;
	std::ofstream fout;
	csv_observer(const std::string& FileName) :fout(FileName){};
	void operator()(const state& x, double t){
		fout << t << "," << x[0] << "," << x[1] << std::endl;
	}
};

int main(){
	lv_system System(2.0, 3.0, 4.0, 5.0);
	lv_system::state State = {1.0,0.5};

	std::cout << "time=0 " << "x=" << State[0] << " " << "y=" << State[1] << std::endl;

	//4ŽŸ‚Ìrunge_kutta–@‚ðŽg‚Á‚Ä‚Ý‚é
	boost::numeric::odeint::runge_kutta4<lv_system::state> Stepper;
	csv_observer Observer("result2.csv");
	boost::numeric::odeint::integrate_adaptive(Stepper, System, State, 0.0, 5.0, 0.05, std::ref(Observer));

	std::cout << "time=10 " << "x=" << State[0] << " " << "y=" << State[1] << std::endl;
}
