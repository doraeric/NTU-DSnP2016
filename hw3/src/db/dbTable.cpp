/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"
#include <cmath>

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
	for(size_t i = 0; i < r._data.size(); i++){
	cout << "here \n";
		if (!r.visible.at(i)){
		//cout << "\nvisible["<<i<<"]: false\n";
	cout << "here 2\n";
			DBTable::printData(os, INT_MAX, false);
		}
		/*if(!r.visible.at(i)){
			os << "    ";
		}*/
		else{
			DBTable::printData(os, r._data.at(i), false);
			//os << setw(4) << r._data.at(i);
		}
		if(i < r._data.size() - 1)
			os << " ";
	}
	os << endl;
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   for(size_t i = 0; i < t._table.size(); ++i){
		for(size_t j = 0; j < t[i].size(); ++j){
//			cout << t[i].visible.size()<<" " << j << endl;
//			cout << t[i][1];
			if(!t[i].visible.at(j)){
				os << "     .";
				//os << setw(6) << t[i][j];
			} else {
				os << setw(6) << t[i][j];
			}
		}
		os << endl;
	}
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
	string multi_line;
	getline(ifs, multi_line);
	multi_line.erase(multi_line.end()-1, multi_line.end());

	size_t lpos; // line pos
	while((lpos = multi_line.find('\r')) != string::npos){
		string line = multi_line.substr(0, lpos);
		multi_line.erase(0, lpos + 1);

		vector<int> i_vector;
		vector<bool> b_vector;
		size_t pos = 0;
		string delimiter = ",";
		while((pos = line.find(delimiter)) != string::npos){
			string s_num = line.substr(0, pos);
			i_vector.push_back(atoi(s_num.c_str()));
			line.erase(0, pos + delimiter.size());
			b_vector.push_back(s_num!="");
		}
		i_vector.push_back(atoi(line.c_str()));
		b_vector.push_back(line!="");


		t._table.push_back(DBRow(i_vector));
		t._table.at(t._table.size()-1).visible = b_vector;
	}

//		for(size_t i = 0; i < this->colnum; i++){
//			cout << cell2int[i] << " ";
//		}cout << endl;
//		for(size_t i = 0; i < this->colnum; i++){
//			cout << vis[i] << " ";
//		}cout << endl;
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
	// TODO
	_data.erase(_data.begin() + c);
	visible.erase(visible.begin() + c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
	// TODO: called as a functional object that compares the data in r1 and r2
	//       based on the order defined in _sortOrder
	size_t n = r1.size() - 1;
	return r1[n] < r2[n];
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   _table.clear();
}

//void addRow(const DBRow& r)
//{
//	// TODO: add a row to the right of the table. Data are in 'd'.
//	for(size_t i = 0; i < _table.nCols(); ++i){
//		if (r[i] == INT_MAX){
//			_table[i].addData(0);
//			_table[i].visible.push_back(false);
//		} else {
//			_table[i].addData(r[i]);
//			_table[i].visible.push_back(true);
//		}
//	}
//}

void
DBTable::addCol(const vector<int>& d)
{
	// TODO: add a column to the right of the table. Data are in 'd'.
	for(size_t i = 0; i < _table.size(); ++i){
		if (d.at(i) == INT_MAX){
			_table[i].addData(0);
			_table[i].visible.push_back(false);
		} else {
			_table[i].addData(d.at(i));
			_table[i].visible.push_back(true);
		}
	}
}

void
DBTable::delRow(int c)
{
	// TODO: delete row #c. Note #0 is the first row.
	_table.erase(_table.begin() + c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t idx) const
{
   // TODO: get the max data in column #c
	bool first = true;
	int max = 0;
	for(size_t i = 0; i < _table.size(); i++){
		if(first){
			if(_table[i].visible[idx]){
				first = false;
				max = _table[i][idx];
			}
			continue;
		}
		if(_table[i].visible[idx] && max < _table[i][idx])
			max = _table[i][idx];
	}
	if(first) return NAN;
   return (float)max;
}

float
DBTable::getMin(size_t idx) const
{
   // TODO: get the min data in column #c
	bool first = true;
	int min = 0;
	for(size_t i = 0; i < _table.size(); i++){
		if(first){
			if(_table[i].visible[idx]){
				first = false;
				min = _table[i][idx];
			}
			continue;
		}
		if(_table[i].visible[idx] && min > _table[i][idx])
			min = _table[i][idx];
	}
	if(first) return NAN;
   return (float)min;
}

float 
DBTable::getSum(size_t idx) const
{
   // TODO: compute the sum of data in column #c
	int sum = 0;
	bool nan = true;
	for(size_t i = 0; i < _table.size(); i++){
		sum += _table[i][idx];
		if(_table[i].visible[idx])
			nan = false;
	}
	if(nan) return NAN;
	return (float)sum;
}

int
DBTable::getCount(size_t idx) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
	vector<int> tmp;
	for(size_t i = 0; i < _table.size(); i++){
		if(_table[i].visible[idx]){
			tmp.push_back(_table[i][idx]);
		}
	}
	int count = tmp.size();
	if(tmp.size()){
		std::sort(tmp.begin(), tmp.end());
		for(size_t i = 0; i < tmp.size()-1; ++i){
			if(tmp.at(i) == tmp.at(i+1))
				count--;
		}
	}
   return count;
}

float
DBTable::getAve(size_t idx) const
{
   // TODO: compute the average of data in column #c
	int sum = 0, n = 0;
	bool nan = true;
	for(size_t i = 0; i < _table.size(); i++){
		sum += _table[i][idx];
		n += (int)_table[i].visible[idx];
		if(_table[i].visible[idx])
			nan = false;
	}
	if(nan) return NAN;
	return (double)sum / (double)n;
}

void
DBTable::sort(const struct DBSort& s)
{
	// TODO: sort the data according to the order of columns in 's'
	for(size_t i = 0; i < s._sortOrder.size(); ++i){
		int idx = s._sortOrder.at(i);
		for(size_t j = 0; j < _table.size(); ++j){
			if(_table[j].visible.at(idx))
				_table[j].addData(_table[j][idx]);
			else
				_table[j].addData(INT_MAX);
			_table[j].visible.push_back(true);
		}
		//std::sort(_table.begin(), _table.end(), DBSort);
		std::sort(_table.begin(), _table.end(), s);
		delCol(nCols()-1);
	}
/*
	for(size_t i = 0; i < s._sortOrder.size(); ++i){
		int col = s._sortOrder.at(i);
//cout << "start sort\n";
		vector<int> tmp;
		for(size_t j = 0; j < _table.size(); ++j){
			if(!_table.at(j).visible.at(col)){
				tmp.push_back(INT_MAX);
			}
			else{
				tmp.push_back(_table.at(j)[col]);
			}
		}
		std::sort(tmp.begin(), tmp.end());
//cout << tmp.size() << endl;
//for(size_t j = 0; j < _table.size(); ++j) cout << tmp.at(j) << " "; cout << endl;
		for(size_t j = 0; j < _table.size(); ++j){
			if(tmp.at(j) == INT_MAX){
				_table.at(j)[col] = 0;
				_table.at(j).visible.at(col) = false;
			}
			else{
				_table.at(j)[col] = tmp.at(j);
				_table.at(j).visible.at(col) = true;
			}
		}
	}
*/
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
	for(size_t i = 0; i < _table.size(); i++){
		if (!_table[i].visible.at(c))
			DBTable::printData(cout, INT_MAX, false);
		else
			DBTable::printData(cout, _table[i][c], false);
		if(i < _table.size()-1)
			cout << " ";
	}
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i].visible[j]) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

