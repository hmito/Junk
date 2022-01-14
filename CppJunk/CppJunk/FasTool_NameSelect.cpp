#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "Fasta.h"
using namespace std;
const string SoftFileName("FasTool_NameSelect");
const string SoftName("FasTool NameSelect v1.1");
//int main(int argc,char** argv){
int main(int argc,char** argv){
	//SoftName�\��
	for(unsigned int i=0;i<SoftName.size()+12;++i)cout<<"=";
	cout<<endl;
	cout<<"===   "<<SoftName<<"   ==="<<endl;
	for(unsigned int i=0;i<SoftName.size()+12;++i)cout<<"=";
	cout<<endl;

	//���O�p�t�@�C���I�[�v��
	ofstream logout((SoftFileName+"_log.txt").c_str());
	if(!logout.is_open()){
		cout<<"*** Error *** : fail to open log file"<<endl;
		cout<<"...wait any key"<<endl;
		getchar();
		return -1;
	}

	//���s���������L�^����
	logout<<SoftName<<endl<<endl;
	logout<<"initial status"<<endl;
	logout<<"\targc\t"<<argc<<endl;
	for(int i=0;i<argc;++i)logout<<"\t"<<argv[i]<<endl;
	logout<<endl;

	//��r�p���C���}�b�v
	typedef std::vector<std::string> Str_vector;
	Str_vector FileNames;

	//�����ɓ��̓t�@�C���������݂��Ȃ��ꍇ
	if(argc<=1){
		logout<<"fail to get input file name"<<endl;
		cout<<">fail to get input file name"<<endl;
		while(true){
			//�t�@�C��������͂�����
			cout<<">please input file name"<<endl;
			cout<<"\t:";
			string str;
			cin>>str;
			FileNames.push_back(str);
			int tmp;
			cout<<"\t finish to input file name? 1:Yes 0:No"<<endl;
			cout<<"\t\t...";
			cin>>tmp;
			if(tmp)break;
		}
	}else{
		logout<<"succeed to get input file name"<<endl;
		//��������t�@�C�������擾
		for(int i=1;i<argc;++i)FileNames.push_back(argv[i]);
	}
	logout<<"\tFileNum\t"<<FileNames.size()<<endl;
	if(FileNames.size()==0){
		logout<<"*** Error *** : input no file"<<endl;
		cout<<"*** Error *** : input no file"<<endl;
		cout<<"...wait any key"<<endl;
		getchar();
		return -1;
	}
	for(unsigned int i=0;i<FileNames.size();++i)logout<<"\t"<<FileNames[i]<<endl;
	logout<<endl;

	//�t�@�C�������ɓǂݏo��
	logout<<"# file open mode"<<endl;
	unsigned int AllDataNum=0;
	typedef multimap<std::string,Fasta::cData> str_fas_multimap;
	typedef pair<std::string,Fasta::cData> str_fas_pair;
	str_fas_multimap Map;
	Str_vector IDs;
	for(Str_vector::iterator itr=FileNames.begin();itr!=FileNames.end();++itr){
		//�t�@�C�����J��
		cout<<">open "<<'"'<<*itr<<'"'<<"...";
		logout<<"open "<<'"'<<*itr<<'"'<<"...";
		ifstream fin(itr->c_str());
		if(!fin.is_open()){
			cout<<"fail"<<endl;
			logout<<"fail"<<endl;
			continue;
		}else{
			cout<<"ok"<<endl;
			logout<<"ok"<<endl;
		}

		if(itr->substr(itr->find_last_of('.'), itr->size() - itr->find_last_of('.'))==string(".txt")){
			cout<<">read name...";
			logout<<"read name...";
			int Cnt=0;
			string Str;
			while(true){
				char c=fin.get();
				if(c=='\t' || c==',' || c=='\n' || fin.eof()){
					if(Str.size()){
						IDs.push_back(Str);
						Str.clear();
						++Cnt;
					}
					if(!fin.good())break;
				}else Str.push_back(c);
			}
			cout<<"ok"<<endl;
			logout<<"ok"<<endl;
			logout<<"\tNameNum\t"<<Cnt<<endl;
			logout<<"\tAllNameNum\t"<<IDs.size()<<endl;
		}else{
			cout<<">read fasta...";
			logout<<"read fasta...";
			//�f�[�^�ǂݏo���p
			Fasta::cData Data;
			//�t�@�C���T�C�Y�擾
			unsigned int DataNum=0;
			//�t�@�C�����[�h
			while(fin.good()){
				//Fasta�f�[�^�Ǎ�
				fin>>Data;
				if(Data.Code.size()==0)break;
				//Map�ɒǉ�
				Map.insert(str_fas_pair(Data.ID,Data));
				//�f�[�^���J�E���g
				++DataNum;
			}
			AllDataNum+=DataNum;
			cout<<"ok"<<endl;
			logout<<"ok"<<endl;
			logout<<"\tDataNum\t"<<DataNum<<endl;
			logout<<"\tAllDataNum\t"<<AllDataNum<<endl;
		}
	}
	logout<<"# end file open mode"<<endl<<endl;

	logout<<"DataNum\t"<<Map.size()<<endl;
	if(Map.size()==0){
		logout<<"*** Error *** : input no data"<<endl;
		cout<<"*** Error *** : input no data"<<endl;
		cout<<"...wait any key"<<endl;
		getchar();
		return -1;
	}

	logout<<"NameNum\t"<<IDs.size()<<endl;
	if(IDs.size()==0){
		logout<<"*** Error *** : input no name"<<endl;
		cout<<"*** Error *** : input no name"<<endl;
		cout<<"...wait any key"<<endl;
		getchar();
		return -1;
	}

	//���ʂ����ɏ����o��
	logout<<"# write result mode"<<endl;
	cout<<">write result...";
	string FileName(FileNames[0]);

	//�������ʂ������o��
	logout<<"open file for result...";
	ofstream foutResult((FileName+".NameSel.txt").c_str());
	if(!foutResult.is_open()){
		logout<<"fail"<<endl;
		logout<<"*** Error *** : fail to open result file."<<endl;
		cout<<"*** Error *** : fail to open result file."<<endl;
		cout<<"...wait any key"<<endl;
		getchar();
		return -1;
	}else logout<<"ok"<<endl;
	logout<<"write result...";
	foutResult<<"FileName"<<endl;
	for(unsigned int i=0;i<FileNames.size();++i)foutResult<<"\t"<<FileNames[i]<<endl;
	foutResult<<"DataNum\t"<<Map.size()<<endl;
	foutResult<<"NameNum\t"<<IDs.size()<<endl<<endl;
	logout<<"ok"<<endl;

	//fas�f�[�^�̌��ʏo�̓t�@�C��
	logout<<"open file for fasta...";
	ofstream foutFasta((FileName+".NameSel.fas").c_str());
	if(!foutFasta.is_open()){
		logout<<"fail"<<endl;
		logout<<"*** Error *** : fail to open fasta file."<<endl;
		logout<<"\t"<<(FileName+".NameSel.fas").c_str()<<endl;
		cout<<"*** Error *** : fail to open fasta file."<<endl;
		cout<<"...wait any key"<<endl;
		getchar();
		return -1;
	}else logout<<"ok"<<endl;

	//���o�̓t�@�C�����J��
	logout<<"write result of fasta...";
	int Cnt=0;
	foutResult<<"fail to find"<<endl;
	for(unsigned int i=0;i<IDs.size();++i){
		std::pair<str_fas_multimap::iterator,str_fas_multimap::iterator> pitr=Map.equal_range(IDs[i]);
		if(pitr.first==pitr.second){
			foutResult<<"\t"<<IDs[i]<<endl;
			continue;
		}
		for(;pitr.first!=pitr.second;++(pitr.first))foutFasta<<pitr.first->second;
		++Cnt;
	}
	if(Cnt==IDs.size())foutResult<<"\t---------"<<endl;
	logout<<"ok"<<endl;
	cout<<"ok"<<endl;
	cout<<"find "<<Cnt<<"/"<<IDs.size()<<endl;
	logout<<"find "<<Cnt<<"/"<<IDs.size()<<endl;
	foutResult<<"find "<<Cnt<<"/"<<IDs.size()<<endl;

	logout<<"# end write result mode"<<endl<<endl;

	cout<<">finish!"<<endl;
	cout<<"...wait any key"<<endl;
	getchar();

	return 0;
}
