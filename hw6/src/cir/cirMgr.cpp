/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <sstream>

using namespace std;

//#define DEBUG_INFO

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
// enum CirParseError moved to header

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
// use objects rather than static,
// all moved to ParserString

/**************************************************************/
/*   class ParserString                                       */
/**************************************************************/
bool
ParserString::parseError(CirErrCode  err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

void ParserString::parse(string & str) {
   if (this->empty()) { throw MISSING_IDENTIFIER; }
   const char printableCh[] = "!\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
   if (this->find_first_of(printableCh) != 0) {
      if (this->at(0) == '\n') throw MISSING_IDENTIFIER;
      if (this->at(0) <= 31 || this->at(0) == 127) {
         errInt = this->at(0);
         throw ILLEGAL_WSPACE;
      }
      throw EXTRA_SPACE;
   }
   size_t delpos = this->find_first_not_of(printableCh);
   if (delpos == string::npos) {
      str = *this;
      this->clear();
   }
   else {
      str = this->substr(0, delpos);
      this->erase(0, delpos);
   }
   colNo += str.size();
}

void ParserString::parse(size_t & num){
   string str;
   try {
      parse(str);
   } catch (CirErrCode  e){
      if (e == MISSING_IDENTIFIER){
         throw MISSING_NUM;
      }
      else
         throw e;
   }
   for(size_t i = 0, s = str.size(); i < s; ++i) {
      if (str[i] < '0' || str[i] > '9'){
         errMsg = str;
         colNo -= str.size();
         throw ILLEGAL_NUM;
      }
   }
   num = atoi(str.c_str());
}

void ParserString::parse(const char del) {
   if (this->empty()) { throw MISSING_SPACE; }
   if (this->at(0) != del ) {
      if (del == ' ') throw MISSING_SPACE;
      if (del == '\n') throw MISSING_NEWLINE;
      this->errMsg = del;
      throw MISSING_IDENTIFIER;
   }
   this->erase(this->begin());
   if (del == '\n') { colNo = 0; ++lineNo; }
   else { ++colNo; }
}

bool ParserString::parseline() {
   size_t pos = this->find_first_of('\n');
   if (pos == string::npos) return false;
   this->erase(0, pos);
   ++lineNo; colNo = 0;
   return true;
}

// check range in Mgr, not in string
void ParserString::parsePI(size_t & idx) {
   try {
      parse(idx);
   } catch (CirErrCode e) {
      if (e == ILLEGAL_NUM) {
         errMsg = "PI literal ID(" + errMsg + ")";
         throw ILLEGAL_NUM;
      }
      if (e == MISSING_NUM) {
         errMsg = "PI";
         throw MISSING_DEF;
      }
      throw e;
   }
   if (idx % 2) {
      errMsg = "PI";
      errInt = idx;
      while (idx) {
         idx /= 10;
         --colNo;
      }
      throw CANNOT_INVERTED;
   }
   idx /= 2;
}

void ParserString::parsePO(size_t & idx) {
   try {
      parse(idx);
   } catch (CirErrCode e) {
      if (e == ILLEGAL_NUM) {
         errMsg = "PO literal ID(" + errMsg + ")";
         throw ILLEGAL_NUM;
      }
      if (e == MISSING_NUM) {
         errMsg = "PO";
         throw MISSING_DEF;
      }
      throw e;
   }
}

void ParserString::parseAIG(size_t & idx) {
   try {
      parse(idx);
   } catch (CirErrCode e) {
      if (e == ILLEGAL_NUM) {
         errMsg = "AIG gate literal ID(" + errMsg + ")";
         throw ILLEGAL_NUM;
      }
      if (e == MISSING_NUM) {
         errMsg = "AIG";
         throw MISSING_DEF;
      }
      throw e;
   }
   if (idx % 2) {
      errMsg = "AIG";
      errInt = idx;
      while (idx) {
         idx /= 10;
         --colNo;
      }
      throw CANNOT_INVERTED;
   }
   idx /= 2;
}

void ParserString::parseAIGin(size_t & idx) {
   try {
      parse(idx);
   } catch (CirErrCode e) {
      if (e == ILLEGAL_NUM) {
         errMsg = "AIG input literal ID(" + errMsg + ")";
         throw ILLEGAL_NUM;
      }
      if (e == MISSING_NUM) {
         errMsg = "AIG input literal ID";
      }
      throw e;
   }
}

bool ParserString::parseSymbol(GateType & gt, size_t & idx, string & symbol) {
   if (this->empty()) return false;
   if (!(this->at(0) == 'i' || this->at(0) == 'o' || this->at(0) == 'c')){
      string c, tmp;
      c.push_back(this->at(0));
      c += "\n";
      ParserString check(c);
      try {
         check.parse(tmp);
      } catch (CirErrCode e) {
         errMsg = check.errMsg;
         errInt = check.errInt;
         if ( e == MISSING_IDENTIFIER) {
            errMsg = "symbol type";
         }
         throw e;
      }
      errMsg = this->at(0);
      throw ILLEGAL_SYMBOL_TYPE;
   }
   if (this->at(0) == 'c') {
      colNo = 1;
      if (this->size() < 2) throw MISSING_NEWLINE;
      if (this->at(1) != '\n') throw MISSING_NEWLINE;
      this->erase(0, 2);
      ++lineNo; colNo = 0;
      return false;
   }
   gt = (this->at(0) == 'i' ? PI_GATE : PO_GATE);
   this->erase(0, 1);
   ++colNo;
   try {
      parse(idx);
   } catch (CirErrCode e) {
      if (e == MISSING_NUM/* || e == EXTRA_SPACE*/){
         errMsg = "symbol index";
         throw MISSING_NUM;
      }
      if (e == ILLEGAL_NUM) {
         errMsg = "symbol index(" + errMsg + ")";
      }
      throw e;
   }
   parse(' ');
   size_t end = this->find_first_of("\n");
   if (end == string::npos || end == 0) {
      errMsg = "symbolic name";
      throw MISSING_IDENTIFIER;
   }
   symbol = this->substr(0, end);
   this->erase(0, end);
   for(size_t i = 0, sz = symbol.size(); i < sz; ++i) {
      if (symbol.at(i) <= 31 || symbol.at(i) ==127) {
         errInt = (int)symbol.at(i);
         throw ILLEGAL_SYMBOL_NAME;
      }
      ++colNo;
   }
   //parse(symbol);
   return true;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
void CirMgr::checkRange(size_t varid, ParserString& strfile) {
   if (varid / 2 > _header.M) {
      strfile.errInt = varid;
      while(varid) {
         --strfile.colNo;
         varid /= 10;
      }
      throw MAX_LIT_ID;
   }
}

void CirMgr::reset() {
   size_t sz = _gateList.size();
   for (size_t i = 0; i < sz; ++i){
      int idx = sz - i - 1;
      delete _gateList[idx];
      _gateList.pop_back();
   }
}

bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream file(fileName.c_str());
   if(!file) {
      cout << "Cannot open design \"" << fileName << "\"!!\n";
      return false;
   }
   stringstream ssfile;
   ssfile << file.rdbuf();
   ParserString strfile(ssfile.str());
   file.close();
   try {
      string identifier;
      try {
      strfile.parse(identifier);
      } catch (CirErrCode e) {
         if (e == MISSING_IDENTIFIER) {
            strfile.errMsg = "aag";
         }
         throw e;
      }
      if (identifier != "aag") { strfile.errMsg = identifier; throw ILLEGAL_IDENTIFIER; }
      size_t M, I, L, O, A;
      strfile.parse(' ');
      int flag = 0;
      try {
         strfile.parse(M);
         ++flag;
         strfile.parse(' ');
         strfile.parse(I);
         ++flag;
         strfile.parse(' ');
         strfile.parse(L);
         ++flag;
         strfile.parse(' ');
         if (L) { strfile.errMsg = "latches"; throw ILLEGAL_NUM; }
         strfile.parse(O); strfile.parse(' ');
         ++flag;
         strfile.parse(A);
      } catch (CirErrCode e) {
         if (e == MISSING_NUM) {
            if (flag == 0) {
               strfile.errMsg = "number of variables";
            } else if (flag == 1) {
               strfile.errMsg = "number of PIs";
//            } else if (flag == 2) {
//               strfile.errMsg = "number of L";
            } else if (flag == 3) {
               strfile.errMsg = "number of POs";
            } else if (flag == 4) {
               strfile.errMsg = "number of AIGs";
            } else {}
         }
         else if (e == ILLEGAL_NUM) {
            if (flag == 0) {
               strfile.errMsg = "number of variables(" + strfile.errMsg + ")";
            } else if (flag == 1) {
               strfile.errMsg = "number of PIs(" + strfile.errMsg + ")";
            } else if (flag == 2) {
//               strfile.errMsg = "number of L(" + strfile.errMsg + ")";
//            } else if (flag == 3) {
               strfile.errMsg = "number of POs(" + strfile.errMsg + ")";
            } else if (flag == 4) {
               strfile.errMsg = "number of AIGs(" + strfile.errMsg + ")";
            } else {}
         }
         throw e;
      }
      if(M < I + A) {
         strfile.errMsg = "Number of variables";
         strfile.errInt = M;
         throw NUM_TOO_SMALL;
      }
      strfile.parse('\n');
      _header.M = M; _header.I = I; _header.L = L; _header.O = O; _header.A = A;
      _gateList.reserve(M + O + 1);
      _PI.reserve(I); _POi.reserve(O);
      _gateList.push_back(new CirGate(this, 0, CONST_GATE));
      for (size_t i = 0; i < M + O; ++i){
         _gateList.push_back(new CirGate(this, i+1));
      }
      for (size_t  i = 0; i < I; ++i){
         size_t idx;
         strfile.parsePI(idx);
         checkRange(idx * 2, strfile);
         try {
            _gateList[idx]->set(PI_GATE, strfile.lineNo + 1);
         } catch (CirErrCode e) {
            if (e == REDEF_CONST) {
               strfile.errGate = _gateList[idx];
               strfile.colNo = 0;
               strfile.errInt = 0;
            } else if (e == REDEF_GATE) {
               strfile.errGate = _gateList[idx];
               strfile.colNo = 0;
               strfile.errInt = idx * 2;
            }
            throw e;
         }
         _PI.push_back(idx);
         strfile.parse('\n');
      }
      /*for ( size_t i = 0; i < L; ++i) {
         strfile.parseline();
      }*/
      for ( size_t i = 0; i < O; ++i) {
         size_t po_fanin;
         strfile.parsePO(po_fanin);
         checkRange(po_fanin, strfile);
         // set PO should never err
         try {
            _gateList[M + 1 + i]->set(PO_GATE, strfile.lineNo + 1, po_fanin);
         } catch (CirErrCode e) {
            if (e == REDEF_CONST) {
               strfile.errGate = _gateList[M + 1 + i];
               strfile.colNo = 0;
               strfile.errInt = 0;
            } else if (e == REDEF_GATE) {
               strfile.errGate = _gateList[M + 1 + i];
               strfile.colNo = 0;
               strfile.errInt = (M + 1 + i) * 2;
            }
            throw e;
         }
         _POi.push_back(po_fanin);
         strfile.parse('\n');
      }
      for ( size_t i = 0; i < A; ++i) {
         size_t idx, in1, in2;
         strfile.parseAIG(idx);
         checkRange(idx * 2, strfile);
         strfile.parse(' ');
         strfile.parseAIGin(in1);
         checkRange(in1, strfile);
         strfile.parse(' ');
         strfile.parseAIGin(in2);
         checkRange(in2, strfile);
         try {
            _gateList[idx]->set(AIG_GATE, strfile.lineNo + 1, in1, in2);
         } catch (CirErrCode e) {
            if (e == REDEF_CONST) {
               strfile.errGate = _gateList[idx];
               strfile.colNo = 0;
               strfile.errInt = 0;
            } else if (e == REDEF_GATE) {
               strfile.errGate = _gateList[idx];
               strfile.colNo = 0;
               strfile.errInt = idx * 2;
            }
            throw e;
         }
         strfile.parse('\n');
      }
      GateType gt;
      size_t idx;
      string symbol;
      while (strfile.parseSymbol(gt, idx, symbol)){
         if (gt == PI_GATE) {
            if (idx >= _header.I) {
               strfile.errMsg = "PI index";
               strfile.errInt = idx;
               throw NUM_TOO_BIG;
            }
            try {
               _gateList[_PI[idx]]->setSymbol(symbol);
            } catch (CirErrCode e) {
               if (e == REDEF_SYMBOLIC_NAME) {
                  strfile.errMsg = "i";
                  strfile.errInt = idx;
               }
               throw e;
            }
         }
         else {
            if (idx >= _header.O) {
               strfile.errMsg = "PO index";
               strfile.errInt = idx;
               throw NUM_TOO_BIG;
            }
            try {
               _gateList[M + 1 + idx]->setSymbol(symbol);
            } catch (CirErrCode e) {
               if (e == REDEF_SYMBOLIC_NAME) {
                  strfile.errMsg = "o";
                  strfile.errInt = idx;
               }
               throw e;
            }
         }
         strfile.parse('\n');
      }
      _comments = strfile;
      if (!_comments.empty() && _comments.at(_comments.size() - 1) != '\n'){
         _comments += "\n";
      }
#ifdef DEBUG_INFO
      for (size_t i = 0; i < M + O + 1; ++i) {
         cout << "\033[1;31m";
         cout << "[" << setw(2) << i << "]";
         _gateList[i]->printInfo();
         cout << "\033[0m";
      }
#endif
#ifdef DEBUG_INFO
      cout << "\033[1;31m";
      cout << "success\n";
      cout << "\033[0m";
#endif
   } catch (CirErrCode  e) {
#ifdef DEBUG_INFO
      cout << "\033[1;31m";
      cout << "exception\n";
      cout << "\033[0m";
#endif
      strfile.parseError(e);
      // destruct when delete Mgr
      return false;
   }
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
#ifdef DEBUG_INFO
   cout << "\033[1;31m";
   cout << __FILE__ << ": " << __func__ << " line:" << __LINE__ << endl;
   cout << "\033[0m";
#endif // DEBUG_INFO
   cout << endl;
   cout << "Circuit Statistics\n"
      << "==================\n"
      << "  PI    " << setw(8) << _header.I << endl
      << "  PO    " << setw(8) << _header.O << endl
      << "  AIG   " << setw(8) << _header.A << endl
      << "------------------\n"
      << "  Total " << setw(8) << _header.I + _header.O + _header.A << endl
      ;
}

void CirMgr::unwalked() const {
   for (size_t i = 0; i < _gateList.size(); ++i) {
      _gateList[i]->_walked = false;
   }
}

void CirMgr::printSubNL(size_t & i, size_t vid) const {
   if (_gateList[vid]->_walked) return;
   _gateList[vid]->_walked = true;
   GateType gtype = _gateList[vid]->getType();
   if (gtype == CONST_GATE || gtype == PI_GATE){
      cout << "[" << i << "] ";
      ++i;
      if (gtype == CONST_GATE) {
         cout << "CONST0\n";
         return;
      }
      cout << "PI  " << vid;
      string s = _gateList[vid]->_symbol;
      if (s != "") {
         cout << " (" << s << ")\n";
      }
      else { cout << endl; }
      return;
   }
   else if (gtype == PO_GATE) {
      size_t data = _gateList[vid]->_fanin1;
      if (_gateList[data/2]->_gtype != UNDEF_GATE) {
         printSubNL(i, data/2);
      }
      cout << "[" << i << "] PO  " << vid << " "
         << (_gateList[data/2]->_gtype == UNDEF_GATE ? "*" : "")
         << (data%2 ? "!" : "") << data/2
         << (_gateList[vid]->_symbol.empty() ? "" : " (" + _gateList[vid]->_symbol + ")")
         << endl;
      ++i;
      return;
   }
   else if (gtype == AIG_GATE) {
      size_t data1 = _gateList[vid]->_fanin1;
      if (_gateList[data1/2]->_gtype != UNDEF_GATE) {
         printSubNL(i, data1/2);
      }
      size_t data2 = _gateList[vid]->_fanin2;
      if (_gateList[data2/2]->_gtype != UNDEF_GATE) {
         printSubNL(i, data2/2);
      }
      cout << "[" << i << "] AIG " << vid << " "
         << (_gateList[data1/2]->_gtype == UNDEF_GATE ? "*" : "")
         << (data1%2 ? "!" : "") << data1/2 << " "
         << (_gateList[data2/2]->_gtype == UNDEF_GATE ? "*" : "")
         << (data2%2 ? "!" : "") << data2/2
         << (_gateList[vid]->_symbol.empty() ? "" : " (" + _gateList[vid]->_symbol + ")")
         << endl;
      ++i;
      return;
   }
   else { return; }
}

void CirMgr::writeAIGs(string & str, size_t vid) const {
   if (_gateList[vid]->_walked) return;
   _gateList[vid]->_walked = true;
   GateType gtype = _gateList[vid]->getType();
   if (gtype == CONST_GATE || gtype == PI_GATE){
      return;
   }
   else if (gtype == PO_GATE) {
      size_t data = _gateList[vid]->_fanin1;
      if (_gateList[data/2]->_gtype != UNDEF_GATE) {
         writeAIGs(str, data/2);
      }
      return;
   }
   else if (gtype == AIG_GATE) {
      size_t data1 = _gateList[vid]->_fanin1;
      if (_gateList[data1/2]->_gtype != UNDEF_GATE) {
         writeAIGs(str, data1/2);
      }
      size_t data2 = _gateList[vid]->_fanin2;
      if (_gateList[data2/2]->_gtype != UNDEF_GATE) {
         writeAIGs(str, data2/2);
      }
      stringstream ss;
      ss << vid * 2 << " " << data1 << " " << data2 << "\n";
      str += ss.str();
      return;
   }
   else { return; }
}

void CirMgr::printNetlist() const {
   unwalked();
   size_t i = 0;
   cout << endl;
   for (size_t oi = _header.M + 1; oi < _gateList.size(); ++oi){
      printSubNL(i, oi);
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (size_t i = 0, sz = _PI.size(); i < sz; ++i) {
      cout << " " << _PI[i];
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (size_t i = 0; i < _header.O; ++i) {
      cout << " " << _header.M + i + 1;
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   vector<int> fl;
   for (size_t i = 1; i < _gateList.size(); ++i) {
      if (_gateList[i]->_gtype == AIG_GATE) {
         size_t vid1, vid2;
         vid1 = _gateList[i]->_fanin1 / 2;
         vid2 = _gateList[i]->_fanin2 / 2;
         if(_gateList[vid1]->_gtype == UNDEF_GATE || _gateList[vid2]->_gtype == UNDEF_GATE) {
            fl.push_back(i);
         }
      }
      else if (_gateList[i]->_gtype == PO_GATE) {
         size_t vid1;
         vid1 = _gateList[i]->_fanin1 / 2;
         if(_gateList[vid1]->_gtype == UNDEF_GATE) {
            fl.push_back(i);
         }
      }
   }
   if (!fl.empty()) {
      cout << "Gates with floating fanin(s):";
      for(size_t i = 0, sz = fl.size(); i < sz; ++i) {
         cout << " " << fl[i];
      }
      cout << endl;
   }
   unwalked();
   for (size_t i = 1, sz = _gateList.size(); i < sz; ++i) {
         size_t vid1, vid2;
         vid1 = _gateList[i]->_fanin1 / 2;
         vid2 = _gateList[i]->_fanin2 / 2;
         _gateList[vid1]->_walked = true;
         _gateList[vid2]->_walked = true;
   }
   vector<int> unused;
   for (size_t i = 1, sz = _header.M; i <= sz; ++i) {
      if(_gateList[i]->_gtype != UNDEF_GATE && !_gateList[i]->_walked) {
         unused.push_back(i);
      }
   }
   if(!unused.empty()) {
      cout << "Gates defined but not used  :";
      for(size_t i = 0, sz = unused.size(); i < sz; ++i) {
         cout << " " << unused[i];
      }
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   string aigs;
   unwalked();
   for (size_t oi = _header.M + 1; oi < _gateList.size(); ++oi){
      writeAIGs(aigs, oi);
   }
   size_t newA = 0;
   for (size_t i = 1, sz = _gateList.size(); i < sz; ++i) {
      if (_gateList[i]->_gtype == AIG_GATE && _gateList[i]->_walked)
         ++newA;
   }
   outfile << "aag " << _header.M << " " << _header.I << " " << _header.L
      << " " << _header.O << " " << newA << endl;
   for (size_t i = 0, sz = _PI.size(); i < sz; ++i){
      outfile << _PI[i] * 2 << endl;
   }
   for (size_t i = 0, sz = _POi.size(); i < sz; ++i) {
      outfile << _POi[i] << endl;
   }
   outfile << aigs;
   for (size_t i = 0, sz = _PI.size(); i < sz; ++i){
      string s = _gateList[_PI[i]]->getSymbol();
      if (!s.empty())
         outfile << "i" << i << " " << s << endl;
   }
   for (size_t i = 0, sz = _POi.size(); i < sz; ++i) {
      string s = _gateList[_header.M + i + 1]->getSymbol();
      if (!s.empty())
         outfile << "o" << i << " " << s << endl;
   }
   bool flag = (&cout == &outfile);
   outfile << "c\n" << _comments;
   if (flag) outfile << "\033[1;37;44m";
   outfile << "output by doraeric.";
   if (flag) outfile << "\033[0m";
   cout << endl;
}
