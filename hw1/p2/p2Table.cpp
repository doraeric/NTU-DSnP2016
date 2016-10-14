#include "p2Table.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <cstdio>	//delete

using namespace std;

// Implement member functions of class Row and Table here

const int Row::operator[] (size_t n) const{
	return _data[n];
}

int& Row::operator[] (size_t n) {
	return _data[n];
}

const Row& Table::operator[] (size_t n) const{
	return _rows.at(n);
}

Row& Table::operator[] (size_t n) {
	return _rows.at(n);
}

bool Table::read(const string& csvFile)
{
	ifstream file(csvFile.c_str());
	if(!file) {
		cout << "failed QQ\n";
		return false;
	}

	string multi_line;
	getline(file, multi_line);
	multi_line.pop_back();
	stringstream ssml(multi_line);

	string line;
	if(getline(ssml, line, '\r')){
		stringstream ssl(line);
		string cell;
		vector<int> cell2int;
		vector<bool> vis;
		while(getline(ssl, cell, ',')){
			cell2int.push_back(stoi(cell));
			vis.push_back(cell!="");
		}
//		for(size_t i = 0; i < cell2int.size(); i++){
//			cout << cell2int.at(i) << " ";
//		}cout << endl;
//		for(size_t i = 0; i < cell2int.size(); i++){
//			cout << vis.at(i) << " ";
//		}cout << endl;
		colnum = cell2int.size();
		int* arr_cell = new int[colnum];
		bool* arr_vis = new bool[colnum];
		copy(cell2int.begin(), cell2int.end(), arr_cell);
		copy(vis.begin(), vis.end(), arr_vis);
		_rows.push_back(Row(arr_vis, arr_cell));
	}

	while(getline(ssml, line, '\r')){
		stringstream ssl(line);
		string cell;
		int* cell2int = new int[colnum];
		bool* vis = new bool[colnum];
		for(size_t i = 0; i < colnum; i++){
			getline(ssl, cell, ',');
			if(cell != "")
				cell2int[i] = stoi(cell);
			else
				cell2int[i] = 0;
			vis[i] = cell != "";
		}
		_rows.push_back(Row(vis, cell2int));
//		for(size_t i = 0; i < this->colnum; i++){
//			cout << cell2int[i] << " ";
//		}cout << endl;
//		for(size_t i = 0; i < this->colnum; i++){
//			cout << vis[i] << " ";
//		}cout << endl;
	}
//	another way to parse
//	vector<int> i_vector;
//	vector<bool> b_vector;
//	size_t pos = 0;
//	while((pos = line.find(delimiter)) != string::npos){
//		string s_num = line.substr(0, pos);
//		i_vector.push_back(atoi(s_num.c_str()));
//		line.erase(0, pos + delimiter.size());
//		if(s_num == "")
//			b_vector.push_back(false);
//		else
//			b_vector.push_back(true);
//	}
//	i_vector.push_back(atoi(line.c_str()));
	/*if(line == "")
		b_vector.push_back(false);
	else
		b_vector.push_back(true);
	int *i_arr = new int[i_vector.size()];
	copy(i_vector.begin(), i_vector.end(), i_arr);
	bool *b_arr = new bool[b_vector.size()];
	copy(b_vector.begin(), b_vector.end(), b_arr);
	if(!this->colnum)
		this->colnum = i_vector.size();
	_rows.push_back(Row(b_arr, i_arr));
*/
	file.close();
	return true;
}

void Table::print(){
	for(size_t i = 0; i < _rows.size(); i++){
		_rows.at(i).print();
	}
}

void Table::add(Row r){
	_rows.push_back(r);
}

void Row::print(){
	for(size_t i = 0; i < sizeof(_data) / sizeof(int); i++)
		cout << _data[i] << " ";
	cout << endl;
}
size_t Table::rownum(){ return _rows.size(); }
