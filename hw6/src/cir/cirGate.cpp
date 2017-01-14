/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/

void CirGate::set(GateType gtype, size_t line, size_t in1, size_t in2) {
   if (_gtype == CONST_GATE) throw REDEF_CONST;
   if (_gtype != UNDEF_GATE) throw REDEF_GATE;
   _gtype = gtype; _lineNum = line; _fanin1 = in1; _fanin2 = in2;
}

void CirGate::setSymbol(const string & symbol) {
   if (_symbol != "") {
      throw REDEF_SYMBOLIC_NAME;
   }
   _symbol = symbol;
}

void CirGate::printFan(int level, string pre, vector<vector<int> > & lists) const {
   if(level <=0) return;
   vector<int> & list = lists.at(_id);
   for(size_t i = 0, sz = list.size(); i < sz; ++i) {
      bool doChild = !_mgr->_gateList[list[i]/2]->_walked;
      size_t id = _mgr->_gateList[list[i]/2]->_id;
      string type = _mgr->_gateList[list[i]/2]->getTypeStr();;
      cout << pre << (list[i]%2 ? "!" : "") << type << " " << id
         << (doChild ? "" : " (*)") << endl;
      _walked = true;
      if(doChild) { _mgr->_gateList[list[i]/2]->printFan(level-1, pre+"  ", lists); }
   }
}

string CirGate::getTypeStr() const {
   if (_gtype == CONST_GATE) {
      return "CONST";
   } else if (_gtype == UNDEF_GATE) {
      return "UNDEF";
   } else if (_gtype == PI_GATE) {
      return "PI";
   } else if (_gtype == PO_GATE) {
      return "PO";
   } else if (_gtype == AIG_GATE) {
      return "AIG";
   }
   return "";
}

void
CirGate::reportGate() const
{
   string type = getTypeStr();
   stringstream ssMsg;
   ssMsg << type << "(" << _id << ")"
      << (_symbol.empty() ? "" : "\"" + _symbol + "\"")
      << ", line " << _lineNum;
   string msg = ssMsg.str();
   size_t len = (46 > msg.size() ? 46 - msg.size() : 0);
   cout << "==================================================\n= "
      << msg << setw(len) << ""
      << " =\n==================================================\n";
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   string type = getTypeStr();
   cout << type << " " << _id << endl;
   if(level == 0) { return; }
   if (!_fin_lists_done) {
      _fin_lists_done = true;
      _fin_lists.reserve(_mgr->_gateList.size());
      for (size_t i = 0, sz = _mgr->_gateList.size(); i < sz; ++i){
         vector<int> list;
         CirGate& gate = *_mgr->_gateList[i];
         if (gate._gtype == CONST_GATE
               || gate._gtype == UNDEF_GATE
               || gate._gtype == PI_GATE) {}
         else if (gate._gtype == AIG_GATE) {
            list.push_back(gate._fanin1);
            list.push_back(gate._fanin2);
         }
         else if (gate._gtype == PO_GATE) {
            list.push_back(gate._fanin1);
         }
         _fin_lists.push_back(list);
      }
#ifdef DEBUG_INFO
      cout << "\033[1;31m";
      for(size_t i = 0; i < _fin_lists.size(); ++i){
         cout << "[" << setw(2) << i << "] ";
         for(size_t j = 0; j < _fin_lists[i].size(); ++j){
            cout << _fin_lists[i][j] << " ";
         }
         cout << endl;
      }
      cout << "\033[0m";
#endif
   } // end of create _fin_lists
   _mgr->unwalked();
   this->printFan( level, "  ", _fin_lists);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   string type = getTypeStr();
   cout << type << " " << _id << endl;
   if(level == 0) { return; }
   if (!_fout_lists_done) {
      _fout_lists_done = true;
      _fout_lists.reserve(_mgr->_gateList.size());
      for(size_t i = 0; i < _mgr->_gateList.size(); ++i) {
         vector<int> list;
         _fout_lists.push_back(list);
      }
      for (size_t i = 0, sz = _mgr->_gateList.size(); i < sz; ++i){
         CirGate& gate = *_mgr->_gateList[i];
         if (gate._gtype == CONST_GATE
               || gate._gtype == UNDEF_GATE
               || gate._gtype == PI_GATE) {}
         else if (gate._gtype == AIG_GATE) {
            _fout_lists[gate._fanin1/2].push_back(gate._id * 2 + gate._fanin1 % 2);
            _fout_lists[gate._fanin2/2].push_back(gate._id * 2 + gate._fanin2 % 2);
         }
         else if (gate._gtype == PO_GATE) {
            _fout_lists[gate._fanin1/2].push_back(gate._id * 2 + gate._fanin1 % 2);
         }
      }
#ifdef DEBUG_INFO
      cout << "\033[1;31m";
      for(size_t i = 0; i < _fout_lists.size(); ++i){
         cout << "[" << setw(2) << i << "] ";
         for(size_t j = 0; j < _fout_lists[i].size(); ++j){
            cout << _fout_lists[i][j] << " ";
         }
         cout << endl;
      }
      cout << "\033[0m";
#endif
   } // end of create _fout_lists
   _mgr->unwalked();
   this->printFan( level, "  ", _fout_lists);
}

