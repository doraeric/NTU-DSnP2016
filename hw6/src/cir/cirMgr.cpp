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

#define DEBUG_INFO

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
// enum CirParseError moved to header

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static CirGate *errGate;

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
   if (this->find_first_of(printableCh) != 0) { throw EXTRA_SPACE; }
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
      if (e == MISSING_IDENTIFIER)
         throw MISSING_NUM;
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

void ParserString::parsePI(size_t & idx) {
   parse(idx);
   if (idx % 2) {
      errMsg = "PI";
      errInt = idx;
      while (idx) {
         idx /= 10;
         --colNo;
      }
      throw CANNOT_INVERTED;
   }
   // TODO check range
   idx /= 2;
}

void ParserString::parsePO(size_t & idx) {
   parse(idx);
   // TODO check range
}


void ParserString::parseAIG(size_t & idx) {
   parse(idx);
   if (idx % 2) {
      errMsg = "AIG";
      errInt = idx;
      while (idx) {
         idx /= 10;
         --colNo;
      }
      throw CANNOT_INVERTED;
   }
   // TODO check range
   idx /= 2;
}


/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream file(fileName.c_str());
   // hw2 p2Table.cpp
   if(!file) {
      cout << "Cannot open design " << fileName << "!!\n";
      return false;
   }
   stringstream ssfile;
   ssfile << file.rdbuf();
   ParserString strfile(ssfile.str());
   file.close();
   try {
      string identifier;
      strfile.parse(identifier);
      if (identifier != "aag") { strfile.errMsg = identifier; throw ILLEGAL_IDENTIFIER; }
      size_t M, I, L, O, A;
      strfile.parse(' ');
      strfile.parse(M); strfile.parse(' ');
      strfile.parse(I); strfile.parse(' ');
      strfile.parse(L); strfile.parse(' ');
      if (L) { strfile.errMsg = "latches"; throw ILLEGAL_NUM; }
      strfile.parse(O); strfile.parse(' ');
      strfile.parse(A); strfile.parse('\n');
      _gateList.reserve(M + O + 1);
      // TODO need delete if fail
      _gateList.push_back(new CirGate(CONST_GATE));
      for (size_t i = 0; i < M + O; ++i){
         _gateList.push_back(new CirGate());
      }
      for (size_t  i = 0; i < I; ++i){
         size_t idx;
         strfile.parsePI(idx);
         _gateList[idx]->set(PI_GATE, strfile.lineNo + 1);
         strfile.parse('\n');
      }
      /*for ( size_t i = 0; i < L; ++i) {
         strfile.parseline();
      }*/
      for ( size_t i = 0; i < O; ++i) {
         size_t po_fanin; // TODO need complete parse for PO, AIG
         strfile.parse(po_fanin);
         _gateList[M + 1 + i]->set(PO_GATE, strfile.lineNo + 1, po_fanin);
         strfile.parse('\n');
      }
      for ( size_t i = 0; i < A; ++i) {
         size_t idx, in1, in2;
         strfile.parseAIG(idx); strfile.parse(' ');
         strfile.parse(in1); strfile.parse(' ');
         strfile.parse(in2);
         _gateList[idx]->set(AIG_GATE, strfile.lineNo + 1, in1, in2);
         strfile.parse('\n');
      }
      for (size_t i = 0; i < M + O + 1; ++i) {
#ifdef DEBUG_INFO
   cout << "\033[1;31m";
         cout << "[" << setw(2) << i << "]";
         _gateList[i]->printInfo();
   cout << "\033[0m";
#endif
      }
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
   cout << "cirMgr.cpp :177\n";
}

void
CirMgr::printNetlist() const
{
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
}

void
CirMgr::writeAag(ostream& /*outfile*/) const
{
}
