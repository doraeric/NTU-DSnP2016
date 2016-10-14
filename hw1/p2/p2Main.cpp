#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include "p2Table.h"

using namespace std;

int main()
{
   Table table;

   // TODO: read in the csv file
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else exit(-1); // csvFile does not exist.

   string cmd;
   while(cin >> cmd){
   	if(cmd == "EXIT") {
		break;
	} else if (cmd == "SUM"){
		int idx, sum = 0;
		cin >> idx;
		for(size_t i = 0; i < table.rownum(); i++){
			sum += table[i][idx];
		}
		cout << "The summation of data in column #" << idx << " is "
			<< sum << ".\n";
	} else if (cmd == "AVE"){
		int idx, sum = 0, n = 0;
		cin >> idx;
		for(size_t i = 0; i < table.rownum(); i++){
			sum += table[i][idx];
			n += (int)table[i].visible[idx];
		}
		//cout << sum << " " << n << endl;
		cout << "The average of data in column #" << idx << " is "
			<< setprecision(1) << fixed << (double)sum / (double)n << ".\n";
	} else if (cmd == "MAX"){
		int idx, max = -99;
		cin >> idx;
		for(size_t i = 0; i < table.rownum(); i++){
			if(table[i].visible[idx] && max < table[i][idx])
				max = table[i][idx];
		}
		cout << "The maximum of data in column #" << idx << " is "
			<< max << ".\n";
	} else if (cmd == "MIN"){
		int idx, min = 100;
		cin >> idx;
		for(size_t i = 0; i < table.rownum(); i++){
			if(table[i].visible[idx] && min > table[i][idx])
				min = table[i][idx];
		}
		cout << "The minimum of data in column #" << idx << " is "
			<< min << ".\n";
	} else if (cmd == "COUNT"){
		int idx, count = 0;
		bool num[200] = {};
		cin >> idx;
		for(size_t i = 0; i < table.rownum(); i++){
			if(table[i].visible[idx])
				num[table[i][idx] + 99] = true;
		}
		for(int i = 0; i < 200; i++){
			count += num[i];
		}
		cout << "The distinct count of data in column #" << idx << " is "
			<< count << ".\n";
	} else if (cmd == "ADD"){
		string input;
		int* add = new int[table.colnum];
		bool* add_vis = new bool[table.colnum];
		for(size_t i = 0; i < table.colnum; i++){
			cin >> input;
			if(input != "-"){
				add[i] = stoi(input);
				add_vis[i] = true;
			}
			else{
				add[i] = 0;
				add_vis[i] = false;
			}
		}
		table.add(Row(add_vis, add));
	} else if (cmd == "PRINT") {
		for(size_t i = 0; i < table.rownum(); i++){
			for(size_t j = 0; j < table.colnum; j++){
				if(!table[i].visible[j]){
					cout << "    ";
				}
				else{
					cout << setw(4) << table[i][j];
				}
			}
			cout << endl;
		}
	} else if (cmd == "p_bool"){
		for(size_t i = 0; i < table.rownum(); i++){
			for (size_t j = 0; j < table.colnum; j++){
			cout << table[i].visible[j] << " ";
			}
			cout << endl;
		}
	} else if (cmd == "p"){
		table.print();
	} else { cout << "Wrong input.\n"; }
   }
   return 0;
}

