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

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
//   CirGate(GateType gt = UNDEF_GATE, size_t f1 = -1, size_t f2 = -1, size_t l = 0):
//      _type(gt), _fanin1(f1), _fanin2(f2), _lineNum(l) {}
   CirGate(GateType t = UNDEF_GATE) : _gtype(t), _lineNum(0), _fanin1(0), _fanin2(0) {};
   ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return _lineNum; }

   // Printing functions
   void printGate() const ;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
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

private:
   GateType _gtype;
   bool _walked;
   size_t _lineNum;
   size_t _fanin1; // 0 = 0, 1 = !0 ...
   size_t _fanin2;

protected:

};

#endif // CIR_GATE_H
