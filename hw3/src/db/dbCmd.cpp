/****************************************************************************
  FileName     [ dbCmd.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include "util.h"
#include "dbCmd.h"
#include "dbTable.h"

// Global variable
DBTable dbtbl;

// Static functions
static bool checkColIdx(const string& token, int& c)
{
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return false;
   }

   if (!myStr2Int(token, c)) {
      cerr << "Error: " << token << " is not a number!!\n";
      return false;
   }
   if (c < 0 || c >= int(dbtbl.nCols())) {
      cerr << "Error: Column index " << c << " is out of range!!\n";
      return false;
   }
   return true;
}

static bool checkRowIdx(const string& token, int& c)
{
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return false;
   }

   if (!myStr2Int(token, c)) {
      cerr << "Error: " << token << " is not a number!!\n";
      return false;
   }
   if (c < 0 || c >= int(dbtbl.nRows())) {
      cerr << "Error: Row index " << c << " is out of range!!\n";
      return false;
   }
   return true;
}

bool
initDbCmd()
{
	// TODO...
	// WHY new Cmd???
	if(cmdMgr->regCmd("DBAPpend"  , 4, new DBAppendCmd) &&
		cmdMgr->regCmd("DBAVerage", 4, new DBAveCmd) &&
		cmdMgr->regCmd("DBCount"  , 3, new DBCountCmd) &&
		cmdMgr->regCmd("DBDelete" , 3, new DBDelCmd) &&
		cmdMgr->regCmd("DBMAx"    , 4, new DBMaxCmd) &&
		cmdMgr->regCmd("DBMIn"    , 4, new DBMinCmd) &&
		cmdMgr->regCmd("DBPrint"  , 3, new DBPrintCmd) &&
		cmdMgr->regCmd("DBRead"   , 3, new DBReadCmd) &&
		cmdMgr->regCmd("DBSOrt"   , 4, new DBSortCmd) &&
		cmdMgr->regCmd("DBSUm"    , 4, new DBSumCmd)
	){ return true; }
	cerr << "From dbCmd.cpp\nRegistering \"init\" commands fails... exiting" << endl;
	return false;
}

//----------------------------------------------------------------------
//    DBAPpend <-Row | -Column> <(int data)...>
//----------------------------------------------------------------------
CmdExecStatus
DBAppendCmd::exec(const string& option)
{
   // TODO...
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
	if (options.size() < 2)
		return CmdExec::errorOption(CMD_OPT_MISSING, "");

	bool doRow = false;
	if (myStrNCmp("-Row", options[0], 2) == 0) doRow = true;
	else if (myStrNCmp("-Column", options[0], 2) != 0)
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
	
	vector<int> ret;
	if(doRow){
		//vector<bool>* tmp = new vector<bool>;
		//vector<bool> &ret2 = *tmp;
		//vector<bool>ret2(dbtbl.nCols());
		vector<bool>ret2;
		//bool tmp[dbtbl.nCols()];
		size_t i;
		for(i = 0; i < options.size() - 1; ++i){
			if(ret.size() >= dbtbl.nCols()) break;
			if(options[i+1] == "-"){
				ret.push_back(0);
				ret2.push_back(false);
				//ret2.at(i) = true;
				//ret2.push_back(options[i+1] != "-");
				//tmp[i] = false;
			} else {
				int n;
				if(!myStr2Int(options[i+1], n)){
					cerr << "Error: “"<< options[i+1]
						<< "” is not an integer!!\n";
					return CMD_EXEC_ERROR;
				}
				ret.push_back(n);
				ret2.push_back(true);
				//ret2.push_back(options[i+1] != "-");
				//tmp[i] = false;
			}
		}
		while(ret.size() < dbtbl.nCols()){
			ret.push_back(0);
			ret2.push_back(false);
			//ret2.at(i) = false;
			//tmp[i] = false;
			i++;
		}
		DBRow r(ret);
		for(size_t i = 0; i < ret2.size(); i++){
			if(ret2.at(i))
				r.visible.push_back(true);
			else
				r.visible.push_back(false);
		}
		dbtbl.addRow(r);
		//cout <<"r2s " << ret2.size() << " ncol " << dbtbl.nCols()<<endl;
		//copy(tmp, tmp+dbtbl.nCols(), ret2.begin());
		//r.visible = ret2;
//		cout << "----------r-------\n";
//		cout << r;
//		cout << "--------tbl-------\n";
//		cout << dbtbl;
		//copy(ret2.begin(), ret2.end(), r.visible);
	} else {
		for(size_t i = 0; i < options.size() - 1; ++i){
			if(ret.size() >= dbtbl.nRows()) break;
			if(options[i+1] == "-"){
				ret.push_back(INT_MAX);
			} else {
				int n;
				if(!myStr2Int(options[i+1], n)){
					cerr << "Error: “"<< options[i+1]
						<< "” is not an integer!!\n";
					return CMD_EXEC_ERROR;
				}
				ret.push_back(n);
			}
		}
		while(ret.size() < dbtbl.nRows()){
			ret.push_back(INT_MAX);
		}
		dbtbl.addCol(ret);
		
	}

   return CMD_EXEC_DONE;
}

void
DBAppendCmd::usage(ostream& os) const
{
   os << "Usage: DBAPpend <-Row | -Column> <(int data)...>" << endl;
}

void
DBAppendCmd::help() const
{
   cout << setw(15) << left << "DBAPpend: "
        << "append a row or column of data to the table" << endl;
}


//----------------------------------------------------------------------
//    DBAVerage <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBAveCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float a = dbtbl.getAve(c);
   ios_base::fmtflags origFlags = cout.flags();
   cout << "The average of column " << c << " is " << fixed
        << setprecision(2) << a << ".\n";
   cout.flags(origFlags);

   return CMD_EXEC_DONE;
}

void
DBAveCmd::usage(ostream& os) const
{     
   os << "Usage: DBAVerage <(int colIdx)>" << endl;
}

void
DBAveCmd::help() const
{
   cout << setw(15) << left << "DBAverage: "
        << "compute the average of a column" << endl;
}


//----------------------------------------------------------------------
//    DBCount <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBCountCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   int n = dbtbl.getCount(c);
   cout << "The distinct count of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBCountCmd::usage(ostream& os) const
{     
   os << "Usage: DBCount <(int colIdx)>" << endl;
}

void
DBCountCmd::help() const
{
   cout << setw(15) << left << "DBCount: "
        << "report the distinct count of data in a column" << endl;
}


//----------------------------------------------------------------------
//    DBDelete <-Row | -Column> <(int index)>
//----------------------------------------------------------------------
CmdExecStatus
DBDelCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options, 2))
      return CMD_EXEC_ERROR;

   bool doRow = false;
   if (myStrNCmp("-Row", options[0], 2) == 0) doRow = true;
   else if (myStrNCmp("-Column", options[0], 2) != 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

   int c;
   if (doRow) {
      if (!checkRowIdx(options[1], c)) return CMD_EXEC_ERROR;
      dbtbl.delRow(c);
   }
   else {
      if (!checkColIdx(options[1], c)) return CMD_EXEC_ERROR;
      dbtbl.delCol(c);
   }

   return CMD_EXEC_DONE;
}

void
DBDelCmd::usage(ostream& os) const
{
   os << "Usage: DBDelete <-Row | -Column> <(int index)>" << endl;
}

void
DBDelCmd::help() const
{
   cout << setw(15) << left << "DBDelete: "
        << "delete a row or column from the table" << endl;
}


//----------------------------------------------------------------------
//    DBMAx <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBMaxCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getMax(c);
   cout << "The max data of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMaxCmd::usage(ostream& os) const
{     
   os << "Usage: DBMAx <(int colIdx)>" << endl;
}

void
DBMaxCmd::help() const
{
   cout << setw(15) << left << "DBMAx: "
        << "report the maximum number of a column" << endl;
}


//----------------------------------------------------------------------
//    DBMIn <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBMinCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getMin(c);
   cout << "The min data of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMinCmd::usage(ostream& os) const
{     
   os << "Usage: DBMIn <(int colIdx)>" << endl;
}

void
DBMinCmd::help() const
{
   cout << setw(15) << left << "DBMIn: "
        << "report the minimum number of a column" << endl;
}


//----------------------------------------------------------------------
//    DBPrint < (int rowIdx) (int colIdx)
//            | -Row (int rowIdx) | -Column (colIdx) | -Table | -Summary>
//----------------------------------------------------------------------
CmdExecStatus
DBPrintCmd::exec(const string& option)
{  
	// TODO...
	vector<string> options;
	if (!CmdExec::lexOptions(option, options))
		return CMD_EXEC_ERROR;
	if (options.empty())
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	
	int doCase = 0; // #row #col
	if      (myStrNCmp("-Row"    , options[0], 2) == 0) doCase = 1;
	else if (myStrNCmp("-Column" , options[0], 2) == 0) doCase = 2;
	else if (myStrNCmp("-Table"  , options[0], 2) == 0) doCase = 3;
	else if (myStrNCmp("-Summary", options[0], 2) == 0) doCase = 4;
	else if (options.size() < 2)
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	else if (options.size() > 2){
		string errstr;
		for(size_t i = 2; i < options.size(); ++i)
			errstr += " " + options.at(i);
		errorOption(CMD_OPT_EXTRA, errstr);
		return CMD_EXEC_ERROR;
	}
	if(doCase == 0){
		int r, c;
		if (!checkRowIdx(options.at(0), r)) return CMD_EXEC_ERROR;
		if (!checkColIdx(options.at(1), c)) return CMD_EXEC_ERROR;
		if(!dbtbl[r].visible[c])
			DBTable::printData(cout, INT_MAX, true);
		else
			DBTable::printData(cout, dbtbl[r][c], true);
		cout << endl;
	} else if (doCase == 1){
		int r;
		if (!checkRowIdx(options.at(1), r)) return CMD_EXEC_ERROR;
		for(size_t i = 0; i < dbtbl[r].size(); ++i){
			if(!dbtbl[r].visible[i])
				DBTable::printData(cout, INT_MAX, false);
			else
				DBTable::printData(cout, dbtbl[r][i], false);
			if(i < dbtbl[r].size()-1)
				cout << " ";
		}
		cout << endl;
	} else if (doCase == 2){
		int c;
		if (!checkColIdx(options.at(1), c)) return CMD_EXEC_ERROR;
		dbtbl.printCol(c);
		cout << endl;
	} else if (doCase == 3){
		cout << dbtbl;
	} else if (doCase == 4){
		dbtbl.printSummary();
	}
	return CMD_EXEC_DONE;
}

void
DBPrintCmd::usage(ostream& os) const
{
   os << "DBPrint < (int rowIdx) (int colIdx)\n"
      << "        | -Row (int rowIdx) | -Column (colIdx) | -Table>" << endl;
}

void
DBPrintCmd::help() const
{
   cout << setw(15) << left << "DBPrint: "
        << "print the data in the table" << endl;
}


//----------------------------------------------------------------------
//    DBRead <(string csvFile)> [-Replace]
//----------------------------------------------------------------------
CmdExecStatus
DBReadCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   bool doReplace = false;
   string fileName;
   for (size_t i = 0, n = options.size(); i < n; ++i) {
      if (myStrNCmp("-Replace", options[i], 2) == 0) {
         if (doReplace) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
         doReplace = true;
      }
      else {
         if (fileName.size())
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         fileName = options[i];
      }
   }

   ifstream ifs(fileName.c_str());
   if (!ifs) {
      cerr << "Error: \"" << fileName << "\" does not exist!!" << endl;
      return CMD_EXEC_ERROR;
   }

   if (dbtbl) {
      if (!doReplace) {
         cerr << "Error: Table exists. Use \"-Replace\" option for "
              << "replacement.\n";
         return CMD_EXEC_ERROR;
      }
      cout << "Table is replaced..." << endl;
      dbtbl.reset();
   }
   if (!(ifs >> dbtbl)) return CMD_EXEC_ERROR;
   cout << "\"" << fileName << "\" was read in successfully." << endl;

   return CMD_EXEC_DONE;
}

void
DBReadCmd::usage(ostream& os) const
{
   os << "Usage: DBRead <(string csvFile)> [-Replace]" << endl;
}

void
DBReadCmd::help() const
{
   cout << setw(15) << left << "DBRead: "
        << "read data from .csv file" << endl;
}


//----------------------------------------------------------------------
//    DBSOrt <(int colIdx)>...
//----------------------------------------------------------------------
CmdExecStatus
DBSortCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   
   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   DBSort sortOrders;
   for (int i = 0, n = options.size(); i < n; ++i) {
      int s;
      if (!checkColIdx(options[i], s)) return CMD_EXEC_ERROR;
      sortOrders.pushOrder(s);
   }
   dbtbl.sort(sortOrders);

   return CMD_EXEC_DONE;
}

void
DBSortCmd::usage(ostream& os) const
{
   os << "Usage: DBSOrt <(int colIdx)>..." << endl;
}

void
DBSortCmd::help() const
{
   cout << setw(15) << left << "DBSort: "
        << "sort the data in the table" << endl;
}


//----------------------------------------------------------------------
//    DBSUm <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBSumCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getSum(c);
   cout << "The sum of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBSumCmd::usage(ostream& os) const
{     
   os << "Usage: DBSUm <(int colIdx)>" << endl;
}

void
DBSumCmd::help() const
{
   cout << setw(15) << left << "DBSUm: "
        << "compute the summation of a column" << endl;
}

