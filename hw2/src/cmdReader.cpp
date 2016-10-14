/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"
#include <iostream>
#include <cstdio>

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);

//----------------------------------------------------------------------
//    tmp func
//----------------------------------------------------------------------
string* _tmp_history = NULL;

//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : if(moveBufPtr(_readBufPtr-1)){ deleteChar(); } break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
	 			memset(_readBuf, 0, READ_BUF_SIZE);
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr + 1); break;
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr - 1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : insertChar(' ', 8-(_readBufPtr-_readBuf)%8); break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY  : /*cout << (int)pch;*/ mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...
   if(ptr == _readBufPtr){ mybeep(); return false; }
   if(ptr < _readBuf || ptr > _readBufEnd){ mybeep(); return false; }
   int n = ptr - _readBufPtr;
   if(n < 0){
   	while (n++){
		cout << '\b';
		_readBufPtr --;
	}
   }
   else if (n > 0){
   	while (n--){
		cout << "\033[C";
		_readBufPtr ++;
	}
   }
   else { mybeep(); return false; }
   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   if(_readBufPtr == _readBufEnd) { mybeep(); return false; }
   
   int cp2end = _readBufEnd - _readBufPtr - 1; // cp2end : length of cursor +1 to end;
   char* tmp = new char[cp2end];
   std::copy(_readBufPtr + 1, _readBufEnd, tmp);
   std::copy(tmp, tmp + cp2end, _readBufPtr);
   _readBufEnd--;
   *_readBufEnd = '\0';

   cout << "\033[K";
   for(int i = 0; i < cp2end; i++){
   	cout << tmp[i];
   }
   for(int i = 0; i < cp2end; i++){
   	cout << "\033[D";
   }
   delete [] tmp;
   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);
   if(_readBufEnd - _readBuf + repeat > READ_BUF_SIZE){
	   repeat = READ_BUF_SIZE - (_readBufEnd - _readBuf);
   }
   if(repeat <= 0) { mybeep(); return; }
   int c2end = _readBufEnd - _readBufPtr; // c2end : length of cursor to end;
   char* tmp = new char[c2end];
   std::copy(_readBufPtr, _readBufEnd, tmp);
   _readBufEnd += repeat;
   for(int i = 0; i < repeat; i++) {
   	cout << ch;
	*_readBufPtr = ch;
	_readBufPtr++;
   }
   for(int i = 0; i < c2end; i++){
   	cout << tmp[i];
   }
   for(int i = 0; i < c2end; i++){
   	cout << "\033[D";
   }
   std::copy(tmp, tmp + c2end, _readBufPtr);
   delete [] tmp;
//   cout << endl;
//   for(int i = 0; i < 10; i++){
//   	cout << _readBuf[i];
//   }
//   cout << endl;
//   printf("\n %x %x %x\n", _readBuf, _readBufPtr, _readBufEnd);
//   cout << _readBuf << " " << _readBufPtr << " " << _readBufEnd << endl;
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
	// TODO...
	if(_readBufPtr != _readBuf){
		moveBufPtr(_readBuf);
	}
	_readBufPtr = _readBufEnd = _readBuf;
	memset (_readBufPtr, 0, READ_BUF_SIZE);
	cout << "\033[K";
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
	// TODO...
	if (index < 0){ index = 0; }
	else if (index > (int)_history.size()){ index = (int)_history.size(); }

	if(index == _historyIdx) { mybeep(); return; }
	//move up
	if(index < _historyIdx) {
		if( _historyIdx == (int)_history.size()){
			_tmp_history = new string(_readBuf);
		}
		_historyIdx = index;
		retrieveHistory();
	}
	// move down
	else {
		if(index == (int)_history.size()){
			if(_tmp_history != NULL){
  deleteLine();
  strcpy(_readBuf, _tmp_history->c_str());
  cout << _readBuf;
  _readBufPtr = _readBufEnd = _readBuf + _tmp_history->size();
  delete _tmp_history;
  _tmp_history = NULL;
  _historyIdx = index;
			} else { mybeep(); return; }
		}
		// normal move down
		else {
			_historyIdx = index;
			retrieveHistory();
		}
	}
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
	// TODO...
	string cppBuf(_readBuf);
	size_t start_pos = cppBuf.find_first_not_of(" ");
	size_t end_pos = cppBuf.find_last_not_of(" ") + 1;
	if (start_pos == string::npos){ return;}
	size_t len = end_pos - start_pos;
	_history.push_back(cppBuf.substr(start_pos, len));
	_historyIdx = _history.size();
	if(_tmp_history != NULL){
		delete _tmp_history;
		_tmp_history = NULL;
	}
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
