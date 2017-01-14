/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;
enum CirErrCode {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

class ParserString : public std::string {
   // substr will lost col and line number
public:
   ParserString(std::string str) : std::string(str), lineNo(0), colNo(0) {}
   void parse(std::string &);
   void parse(size_t &);
   void parse(const char delimiter);
   void parsePI(size_t &);
   void parsePO(size_t &);
   void parseAIG(size_t &);
   void parseAIGin(size_t &);
   bool parseSymbol(GateType &, size_t & idx, string & symbol);
   bool parseline();
   size_t lineNo;
   size_t colNo;
   bool parseError(CirErrCode);
   string errMsg;
   int errInt;
   CirGate *errGate;
};


class CirMgr
{
public:
   CirMgr() {}
   ~CirMgr() { reset(); } 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { return 0; }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   // varid => 2:1 3:!1 4:2 ...
   /*add*/ void checkRange(size_t varid, ParserString &);
   /*add*/ void reset();

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   /*add*/ void unwalked() const;
   /*add*/ void printSubNL(size_t & i, size_t vid) const;
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   /*add*/void writeAIGs(string &, size_t varid) const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   friend class CirGate;

private:
   ofstream           *_simLog;
   vector<CirGate*> _gateList;
   struct Header { size_t M, I, L, O, A; } _header;
   vector<size_t> _PI;
   vector<size_t> _POi;
   string _comments;

};

#endif // CIR_MGR_H
