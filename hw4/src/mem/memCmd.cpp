/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"
#include "rnGen.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
	// TODO
	// check option
	vector<string> options;
	if (!CmdExec::lexOptions(option, options))
		return CMD_EXEC_ERROR;
	if (options.empty())
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	bool arr = false;
	int count[] = {1, 0};
	int nobj, narr;
	for (size_t i = 0, n = options.size(); i < n; ++i) {
		if (count[0]){
			if(!myStr2Int(options[i], nobj)) // var
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			count[0]--;
		} else if (count[1]){
			if(!myStr2Int(options[i], narr)) // var
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			count[1]--;
		} else if (!myStrNCmp("-Array" , options[i], 2)) {
			arr = true;
			count[1]   = 1; // var
		} else { return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]); }
	} if(count[0] || count[1]) return CmdExec::errorOption(CMD_OPT_MISSING, "");

	/*=========================================
	vector<string> options;
	bool doArr = false;
	int num = 0, arr = 0;

	if (!CmdExec::lexOptions(option, options))
		return CMD_EXEC_ERROR;
	if (options.empty())
		return CmdExec::errorOption(CMD_OPT_MISSING, "numObjects");
	else if (options.size() == 1){
		if (!myStr2Int(options[0], num)) {
			return CMD_EXEC_ERROR;
		}
	}
	else if (options.size() < 4){
		if (myStrNCmp("-Array", options[1], 2) == 0) doArr = true;
		else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
		if (options.size() < 3)
			return CmdExec::errorOption(CMD_OPT_MISSING, "arraySize");
		if (!myStr2Int(options[2], arr)) return CMD_EXEC_ERROR;
	}
	else {
		string errstr;
		for(size_t i = 2; i < options.size(); ++i)
			errstr += " " + options.at(i);
		errorOption(CMD_OPT_EXTRA, errstr);
		return CMD_EXEC_ERROR;
	}
	================================================*/

	try{
		if(!arr)	mtest.newObjs(nobj);
		else	mtest.newArrs(nobj, narr);
	}
	catch (bad_alloc& e){
		return CMD_EXEC_ERROR;
	}
	return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
	// TODO

	// check option
	vector<string> options;
	if (!CmdExec::lexOptions(option, options))
		return CMD_EXEC_ERROR;
	if (options.empty())
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	bool ind = false, ran = false, arr = false;
	bool group[] = {false, false};
	int count = 0;
	int id;
	for (size_t i = 0, n = options.size(); i < n; ++i) {
		if (count){
			if(!myStr2Int(options[i], id)) // var
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			count--;
		} else if (!myStrNCmp("-Index" , options[i], 2)) {
			int gid = 0; // var
			count   = 1; // var
			if(group[gid]) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
			group[gid] = true;
			ind = true;
		} else if (!myStrNCmp("-Random", options[i], 2)) {
			int gid = 0; // var
			count   = 1; // var
			if(group[gid]) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
			group[gid] = true;
			ran = true;
		} else if (!myStrNCmp("-Array" , options[i], 2)) {
			int gid = 1; // var
			count   = 0; // var
			if(group[gid]) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
			group[gid] = true;
			arr = true;
		} else { return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]); }
	} if(count) return CmdExec::errorOption(CMD_OPT_MISSING, "");
	if(!(ind || ran)) return CmdExec::errorOption(CMD_OPT_MISSING, "");

	if(ind && arr){
		if(id >= 0 && id < (int)mtest.getArrListSize())
			mtest.deleteArr(id);
	}
	else if (ind){
		if(id >= 0 && id < (int)mtest.getObjListSize())
			mtest.deleteObj(id);
	}
	else {
		if(id >0)
		for (int i = 0; i < id; ++i){
			int randnum = rnGen(0);
			if(arr){
				if(randnum >= 0 && randnum < (int)mtest.getArrListSize())
					mtest.deleteArr(randnum);
			}
			else {
				if(randnum >= 0 && randnum < (int)mtest.getObjListSize())
					mtest.deleteObj(randnum);
			}
		}
	}
		

	return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


