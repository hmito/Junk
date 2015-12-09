#ifndef CSVITERATOR_INC
#define CSVITERATOR_INC 100
#
#include<iterator>
#include<iostream>
#include<sstream>

template<class Elem = char, class Traits = std::char_traits<Elem> >
struct basic_csv_mode{
public:
	Elem Sep;
	Elem End;
	Elem Esc;
public:
	constexpr basic_csv_mode(Elem Sep_, Elem End_ = '\n', Elem Esc_ = '"')
		: Sep(Sep_)
		, End(End_)
		, Esc(Esc_){}
};

namespace csv{
	namespace mode{
		constexpr const basic_csv_mode<char> csv(',');
		constexpr const basic_csv_mode<char> tsv('\t');
		constexpr const basic_csv_mode<char> ssv(' ');
	}

	template<class Elem = char, class Traits = std::char_traits<Elem> >
	std::basic_string < Elem, Traits > cell_encode(std::basic_string < Elem, Traits > Str, basic_csv_mode<Elem, Traits> CSVMode){
		if(Str.find(CSVMode.Sep) < Str.size() || Str.find(CSVMode.Esc) < Str.size()){
			for(auto itr = Str.begin(); itr != Str.end(); ++itr){
				if(*itr == CSVMode.Esc){
					itr = Str.insert(++itr, CSVMode.Esc);
				}
			}

			Str.insert(Str.begin(), CSVMode.Esc);
			Str.push_back(CSVMode.Esc);
		}

		return std::move(Str);
	}
	template<class Elem = char, class Traits = std::char_traits<Elem> >
	std::basic_string < Elem, Traits > cell_decode(std::basic_string < Elem, Traits > Str, basic_csv_mode<Elem, Traits> CSVMode){
		if(Str.size()>1 && Str.front() == CSVMode.Esc && Str.back() == CSVMode.Esc){
			Str.erase(Str.begin());
			Str.pop_back();

			bool EscFlag = false;
			for(auto itr = Str.begin(); itr != Str.end(); ++itr){
				if(*itr == CSVMode.Esc){
					if(EscFlag){
						itr = Str.erase(itr);
						--itr;
						EscFlag = false;
					} else{
						EscFlag = true;
					}
				} else{
					EscFlag = false;
				}
			}
		}

		return std::move(Str);
	}
}

template<class Elem = char, class Traits = std::char_traits<Elem> >
class basic_ocsv_iterator : public std::iterator < std::output_iterator_tag, void, void, void, void >{
private:
	using my_type = basic_ocsv_iterator < Elem, Traits >;
	using ostream_type = std::basic_ostream < Elem, Traits >;
	using string_type = std::basic_string < Elem, Traits >;
	using csv_mode_type = basic_csv_mode<Elem, Traits>;
private:
	struct output_cell{
	private:
		my_type& My;
	private:
		void cell_head_treat(){
			if(My.IsCellHead && !My.IsLineHead){
				*(My.pstream) << My.CSVMode.Sep;
			}
			My.IsCellHead = false;
		}
	public:
		output_cell(my_type& My_) :My(My_){}
		void operator=(const string_type& Str){
			cell_head_treat();
			*(My.pstream) << csv::cell_encode(Str, My.CSVMode);
		}
		void raw_str(const string_type& Str){
			cell_head_treat();
			*(My.pstream) << Str;
		}
		template<typename T>
		output_cell& operator<<(const T& Val){
			cell_head_treat();
			*(My.pstream) << Val;
			return *this;
		}
		template<typename T>
		void write(const T& Val){
			operator<<(Val);
		}
		friend std::basic_istream<Elem, Traits>& operator>>(std::basic_istream<Elem, Traits>& in, output_cell p){
			string_type Str;
			in >> Str;
			p = Str;
			return in;
		}
	};
private:
	ostream_type* pstream;
	csv_mode_type CSVMode;
	bool IsCellHead;
	bool IsLineHead;
public:
	basic_ocsv_iterator()
		: pstream(nullptr){}
	basic_ocsv_iterator(ostream_type& Stream_, csv_mode_type CSVMode_)
		: pstream(&Stream_)
		, CSVMode(CSVMode_)
		, IsCellHead(false)
		, IsLineHead(true){}
	basic_ocsv_iterator(const my_type& My_) = default;
	my_type& operator=(const my_type& My_) = default;
public:
	operator bool()const{ return pstream; }
	output_cell operator*(){ return output_cell(*this); }
	my_type& operator++(){
		IsCellHead = true;
		IsLineHead = false;
		return *this;
	}
	my_type operator++(int){
		my_type ans(*this);
		operator++();
		return ans;
	}
	bool eol()const{ return IsLineHead; }
	void endl(){
		(*pstream) << CSVMode.End;
		IsLineHead = true;
		IsCellHead = false;
	}
};
using ocsv_iterator = basic_ocsv_iterator < char, std::char_traits<char> >;

template<class Elem = char, class Traits = std::char_traits<Elem> >
class basic_icsv_iterator : public std::iterator < std::input_iterator_tag, std::basic_string<Elem, Traits>>{
private:
	using my_type = basic_icsv_iterator < Elem, Traits >;
public:
	using sstream_type = std::basic_stringstream < Elem, Traits >;
	using istream_type = std::basic_istream < Elem, Traits >;
	using pos_type = typename istream_type::pos_type;
	using string_type = std::basic_string < Elem, Traits >;
	using csv_mode_type = basic_csv_mode<Elem, Traits>;
private:
	istream_type* pstream;
	csv_mode_type CSVMode;
	pos_type Pos;
	sstream_type sstream;
	bool HasRead;
	pos_type ReadPos;
	bool IsLineHead;
private:
	void next(){
		if(HasRead){
			HasRead = false;
			Pos = ReadPos;
		} else{
			Elem c;
			pstream->seekg(0, std::ios::end);
			auto EndPos = pstream->tellg();
			pstream->seekg(Pos);

			bool EscFlag = false;
			while(pstream->tellg() != EndPos){
				c = pstream->get();

				if(c == CSVMode.End || c == EOF){
					IsLineHead = true;
					break;
				}

				if(c == CSVMode.Sep && !EscFlag){
					IsLineHead = false;
					break;
				}

				if(c == CSVMode.Esc){
					EscFlag = !EscFlag;
				}
			}

			Pos = pstream->tellg();
		}
	}
	sstream_type& read(){
		if(!HasRead){
			Elem c;
			sstream.str(string_type());
			sstream.clear();

			pstream->seekg(0, std::ios::end);
			auto EndPos = pstream->tellg();
			pstream->seekg(Pos);

			bool EscFlag = false;
			while(pstream->tellg() != EndPos){
				c = pstream->get();

				if(c == CSVMode.End || c == EOF){
					IsLineHead = true;
					break;
				}

				if(c == CSVMode.Sep && !EscFlag){
					IsLineHead = false;
					break;
				}

				if(c == CSVMode.Esc){
					EscFlag = !EscFlag;
				}

				sstream << c;
			}

			ReadPos = pstream->tellg();
			HasRead = true;
		}
		return sstream;
	}
private:
	struct input_cell{
	private:
		my_type& My;
	public:
		input_cell(my_type& My_) :My(My_){}
		operator string_type(){ return csv::cell_decode(My.read().str(), My.CSVMode); }
		string_type raw_str()const{ return My.read().str(); }
		template<typename T>
		input_cell& operator>>(T& Val){
			My.read() >> Val;
			return *this;
		}
		template<typename T>
		T read(){
			T Val;
			operator>>(Val);
			return Val;
		}
		friend std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& out, input_cell p){
			return out << static_cast<string_type>(p);
		}
		friend bool operator==(const input_cell& in, const string_type& str){return in.raw_str() == str;}
		friend bool operator!=(const input_cell& in, const string_type& str){ return in.raw_str() != str; }
	};
public:
	basic_icsv_iterator() :pstream(nullptr){}
	basic_icsv_iterator(istream_type& Stream_, pos_type Pos_, csv_mode_type CSVMode_, bool IsLineHead_)
		: pstream(&Stream_)
		, CSVMode(CSVMode_)
		, Pos(Pos_)
		, sstream()
		, HasRead(false)
		, IsLineHead(IsLineHead_){}
	basic_icsv_iterator(const my_type& My_)
		: pstream(My_.pstream)
		, CSVMode(My_.CSVMode)
		, Pos(My_.Pos)
		, sstream(My_.sstream.str())
		, HasRead(My_.HasRead)
		, ReadPos(My_.ReadPos)
		, IsLineHead(My_.IsLineHead){}
	my_type& operator=(const my_type& My_){
		if(this != &My_){
			pstream = My_.pstream;
			CSVMode = My_.CSVMode;
			Pos = My_.Pos;
			sstream = My_.sstream.str();
			HasRead = My_.HasRead;
			ReadPos = My_.ReadPos;
			IsLineHead = My_.IsLineHead;
		}
	}
	input_cell operator*(){ return input_cell(*this); }
	my_type& operator++(){
		next();
		return *this;
	}
	my_type operator++(int){
		basic_icsv_iterator ans(*this);
		operator++();
		return ans;
	}
	bool eol()const{ return IsLineHead; }
	bool eof(){
		pstream->seekg(0, std::ios::end);
		return Pos == pstream->tellg();
	}
	pos_type pos()const{ return Pos; }
	friend bool operator==(const my_type& my1, const my_type& my2){
		if(&my1 == &my2)return true;

		if(my1.pstream != my2.pstream) return false;

		return my1.Pos == my2.Pos;
	}
	friend bool operator!=(const my_type& my1, const my_type& my2){ return !(my1 == my2); }
};
using icsv_iterator = basic_icsv_iterator < char, std::char_traits<char> >;

namespace csv{
	namespace in{
		template<class Elem = char, class Traits = std::char_traits<Elem> >
		inline basic_ocsv_iterator<Elem, Traits> begin(std::basic_ostream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
			Stream_.seekp(0, std::ios::beg);
			return basic_ocsv_iterator<Elem, Traits>(Stream_, CSVMode_);
		}
		template<class Elem = char, class Traits = std::char_traits<Elem> >
		inline basic_ocsv_iterator<Elem, Traits> current(std::basic_ostream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
			return basic_ocsv_iterator<Elem, Traits>(Stream_, CSVMode_);
		}
		template<class Elem = char, class Traits = std::char_traits<Elem> >
		inline basic_ocsv_iterator<Elem, Traits> end(std::basic_ostream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
			Stream_.seekp(0, std::ios::end);
			return basic_ocsv_iterator<Elem, Traits>(Stream_, CSVMode_);
		}
	}

	namespace out{
		template<class Elem = char, class Traits = std::char_traits<Elem> >
		inline basic_icsv_iterator<Elem, Traits> begin(std::basic_istream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
			Stream_.seekg(0, std::ios::beg);
			auto Pos = Stream_.tellg();
			return basic_icsv_iterator<Elem, Traits>(Stream_, Pos, CSVMode_, true);
		}
		template<class Elem = char, class Traits = std::char_traits<Elem> >
		inline basic_icsv_iterator<Elem, Traits> current(std::basic_istream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
			auto Pos = Stream_.tellg();
			return basic_icsv_iterator<Elem, Traits>(Stream_, Pos, CSVMode_, false);
		}
		template<class Elem = char, class Traits = std::char_traits<Elem> >
		inline basic_icsv_iterator<Elem, Traits> end(std::basic_istream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
			Stream_.seekg(0, std::ios::end);
			auto Pos = Stream_.tellg();
			return basic_icsv_iterator<Elem, Traits>(Stream_, Pos, CSVMode_, true);
		}
	}

	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline basic_ocsv_iterator<Elem, Traits> begin(std::basic_ostream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
		return in::begin(Stream_, CSVMode_);
	}
	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline basic_ocsv_iterator<Elem, Traits> current(std::basic_ostream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
		return in::current(Stream_, CSVMode_);
	}
	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline basic_ocsv_iterator<Elem, Traits> end(std::basic_ostream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
		return in::end(Stream_, CSVMode_);
	}

	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline basic_icsv_iterator<Elem, Traits> begin(std::basic_istream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
		return out::begin(Stream_, CSVMode_);
	}
	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline basic_icsv_iterator<Elem, Traits> current(std::basic_istream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
		return out::current(Stream_, CSVMode_);
	}
	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline basic_icsv_iterator<Elem, Traits> end(std::basic_istream < Elem, Traits >& Stream_, basic_csv_mode < Elem, Traits > CSVMode_ = csv::mode::csv){
		return out::end(Stream_, CSVMode_);
	}

	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline void advance_line(basic_ocsv_iterator<Elem, Traits>& itr, unsigned int num = 1){
		for(unsigned int cnt = 0; cnt < num; ++cnt)itr.endl();
	}
	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline basic_ocsv_iterator<Elem, Traits> next_line(const basic_ocsv_iterator<Elem, Traits>& itr, unsigned int num = 1){
		basic_ocsv_iterator<Elem, Traits> next_itr(itr);
		advance_line(next_itr, num);
		return next_itr;
	}

	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline void advance_line(basic_icsv_iterator<Elem, Traits>& itr, unsigned int num = 1){
		unsigned int no = 0;
		do{
			++itr;
			if(itr.eol())++no;
		} while(no < num);
	}
	template<class Elem = char, class Traits = std::char_traits<Elem> >
	inline basic_icsv_iterator<Elem, Traits> next_line(const basic_icsv_iterator<Elem, Traits>& itr, unsigned int num = 1){
		basic_icsv_iterator<Elem, Traits> next_itr(itr);
		advance_line(next_itr, num);
		return next_itr;
	}
}
#
#endif
