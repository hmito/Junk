#include<array>
#include<fstream>
#include<boost/numeric/odeint.hpp>
#include<hmLib_v4_01/hmLib/csv_iterator.hpp>
namespace odeint = boost::numeric::odeint;

struct SEIR_system{
	using state = std::array<double, 4>;
	enum state_category{S=0,E=1,I=2,R=3};
	double beta;	//ä¥êıó¶
	double gamma;	//î≠è«ó¶
	double r;		//é°ñ¸ó¶
	double m;		//ñ∆âuëré∏ó¶
	SEIR_system(double beta_, double gamma_, double r_, double m_)
		: beta(beta_)
		, gamma(gamma_)
		, r(r_)
		, m(m_){
	}
	void operator()(const state& x, state& dxdt, double t){
		double beta2 = beta*std::pow(sin(2 * acos(-1) * t / 100),2.);
		dxdt[S] = -beta2* x[S] * x[I] + m * x[R];
		dxdt[E] = beta2 * x[S] * x[I] - gamma*x[E];
		dxdt[I] = gamma*x[E] - r *x[I];
		dxdt[R] = r * x[I] - m * x[R];
	}
};

template<typename state_type,typename time_type = double>
struct csv_observer{
	hmLib::ocsv_iterator ocsv;
	csv_observer(std::ostream& out_,const hmLib::basic_csv_mode<>& csv_mode = hmLib::csv::mode::csv)
		:ocsv(hmLib::ocsv_iterator(out_, csv_mode)){		
	}
	void operator()(const state_type& x, const double t){
		(*ocsv++) << t;
		for(auto& i : x)(*ocsv++) << i;
		ocsv.endl();
	}
};
int main(void){
	using system = SEIR_system;
	using state = system::state;

	state Init{0.99,0.01,0,0};
	system System(0.3, 0.3, 0.1, 0.002);

	auto Stepper = odeint::runge_kutta4<SEIR_system::state>();//odeint::make_controlled(0.0001,0.0001,odeint::runge_kutta_dopri5<SEIR_system::state>());

	std::ofstream fout("result.csv");
	csv_observer<SEIR_system::state> Observer(fout);

	odeint::integrate_const(Stepper, System, Init, 0.0, 10000.0, 1.0, std::ref(Observer));

	return 0;
}
