#ifndef INC_FASTA
#define INC_FASTA 100
#
#include <iostream>
#include <string>
#include <sstream>
namespace Fasta{
	struct cData{
	public:
		std::string ID;
		std::string Code;
	public:
		cData(){return;}
		cData(const std::string& ID_,const std::string& Code_):ID(ID_),Code(Code_){return;}
		friend std::ostream& operator<<(std::ostream& out,const cData& Data){
			out<<">"<<Data.ID<<"\n"<<Data.Code<<"\n";
			return out;
		}
		friend std::istream& operator>>(std::istream& in,cData& Data){
			Data.ID.clear();
			Data.Code.clear();
			while(in.get()!='>'){
				if(!in.good())return in;
			}
			while(true){
				char c=in.get();
				if(c=='\n' || (!in.good()))break;
				else Data.ID.push_back(c);
			}
			while(true){
				char c=in.get();
				if((!in.good()) || c=='>'){
					in.putback(c);
					break;
				}
				if(c=='\n')continue;
				Data.Code.push_back(c);
			}
			return in;
		}
	};
	inline std::string utos_10order(unsigned int u) {
		static char SizeChar[10] = { '_','A','B','C','D','E','F','G','H','I' };
		unsigned int un = 0;
		while (u > 10 && un < 10) {
			u /= 10;
			++un;
		}
		if (un >= 10)return "--";
		std::stringstream sout;
		sout << SizeChar[un] << u;
		return sout.str();
	}
}
#
#endif
