/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip> // setw
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
   CirGate(CirMgr* mgr, size_t id, GateType t = UNDEF_GATE) :
      _mgr(mgr), _id(id), _gtype(t), _lineNum(0), _fanin1(0), _fanin2(0),
      _fin_lists_done(false), _fout_lists_done(false) {};
   ~CirGate() {}

   // Basic access methods
   GateType getType() const { return _gtype; }
   string getSymbol() const { return _symbol; }
// not sure what string should this return for gate CONST, UNDEF, PO
   string getTypeStr() const;
   unsigned getLineNo() const { return _lineNum; }
   bool isAig() const { return false; }

   // Printing functions
   void printGate() const {} // not implement
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void printFan(int level, string pre, vector<vector<int> > & lists) const;
   void setSymbol(const string &);
   void set(GateType type,size_t line, size_t in1 = 0, size_t in2 = 0);
   void printInfo(){
      string type;
      if (_gtype == PI_GATE) type = "PI   ";
      else if (_gtype == PO_GATE) type = "PO   ";
      else if (_gtype == AIG_GATE) type = "AIG  ";
      else if (_gtype == CONST_GATE) type = "CONST";
      else if (_gtype == UNDEF_GATE) type = "UNDEF";
      std::cout << "walked: " << (_walked ? "T " : "F ")
         << "GateType: " << type << " fin: (" << setw(2) << _fanin1
         << ", " << setw(2) << _fanin2 << ") _lineNum: " << _lineNum << std::endl;
   }
   friend class CirMgr;

private:
   CirMgr * _mgr;
   size_t _id;
   GateType _gtype;
   size_t _lineNum;
   size_t _fanin1; // 0 = 0, 1 = !0 ...
   size_t _fanin2;
   mutable bool _walked;
   string _symbol;
   mutable bool _fin_lists_done, _fout_lists_done;
   mutable vector<vector<int> > _fin_lists, _fout_lists;
};

#endif // CIR_GATE_H
