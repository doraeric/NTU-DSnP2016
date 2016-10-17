/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
	// TODO...
	_dofile = new ifstream(dof.c_str());
	if(!_dofile) { // equals to _dofile.fail()
		cerr << "failed QQ\n";
		return false;
	}
	return true;
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
	if(_dofile == 0) return;
	assert(_dofile != 0);
	// TODO...
	_dofile->close();
	delete _dofile;
	_dofile = NULL;
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);

   // execute the command
   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

/*bool CmdParser::readCmd(istream& input){
	cout << "QQ WTF!!!!!!!!!!!!!!!!\n";
	if (_dofile != 0){ // read from file
		//getline(input, ???);
	}
	else { // read from cin
	}
	return false;
}*/

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   // TODO...
	for(CmdMap::const_iterator it = _cmdMap.begin(); it != _cmdMap.end(); it++){
		it->second->help();
	}
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
   assert(_tempCmdStored == false);
   assert(!_history.empty());
   string str = _history.back();

	// TODO... Exception
	assert(str[0] != 0 && str[0] != ' ');
	CmdExec* cmd_exe = NULL;
	string cmd_str;
	size_t del_pos = str.find_first_of(" "); // delimiter position
	if(del_pos == string::npos){
		option = "";
		cmd_str = str;
	} else {
		cmd_str = str.substr(0, del_pos);
	}
	size_t opt_pos = str.find_first_not_of(" ", del_pos);
	if(opt_pos == string::npos){
		option = "";
	} else {
		option = str.substr(opt_pos);
	}
	cmd_exe = getCmd(cmd_str);
	if(cmd_exe == 0){
		cerr << "Illegal command!! (" << cmd_str << ")\n";
		return NULL;
	}
	return cmd_exe;
	return NULL;
}

size_t myStrGetTok2(const string& str,
 string& tok, size_t pos = 0, const char del = ' ');
bool getCmdList(const string& cmd, vector<string>& cmds,
 const vector<string>& check_list){
	for(size_t li = 0; li < check_list.size(); ++li){
		// every loop
		string stdcmd = check_list.at(li);
		if(cmd.size() > stdcmd.size()) continue;
//		bool multi_cmd = false;
//		if(cmd.size() < stdcmd.find_last_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ")+1)
//			multi_cmd = true;
		bool check_to_end = false;
		bool is_cmd = true;
		for (unsigned i = 0; i < stdcmd.size(); ++i) {
			if (i == cmd.size()){
				break;
			}
			char ch1 = tolower(cmd[i]);
			char ch2 = tolower(stdcmd[i]);
			if(ch1 != ch2){
				is_cmd = false;
				break;
			}
			if(i == stdcmd.size() - 1){
				check_to_end = true;
			}
		}
		if(is_cmd) cmds.push_back(stdcmd);
		if(check_to_end) return true;
	}
	return false; // true if completely the same
}
// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. [Before] Null cmd
//    cmd> $
//    -----------
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    -----------
//    [Before] partially matched (multiple matches)
//    cmd> h$aaa                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$aaa                // and then re-print the partial command
//
// 3. [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $
//    -----------
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$hahah
//    [After Tab]
//    cmd> heLp $hahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//
// 4. [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. [Before] Already matched
//    cmd> help asd$fg
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$fg
//
// 6. [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
//

void
CmdParser::listCmd(const string& str)
{
	// TODO...
	// Case 1: list all
	string cmd;
	size_t cmd_end_pos = myStrGetTok2(str, cmd);
	size_t cur_pos = _readBufPtr - _readBuf;

	if(cmd_end_pos == string::npos){
		int count = 0;
		for(CmdMap::iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it, ++count){
			string cmd = it->first + it->second->getOptCmd();
			if(!(count % 5)) cout << endl;
				cout << setw(12) << left << cmd;
		}
		reprintCmd();
		return;
	}

	vector<string>cmd_list;
	for(CmdMap::iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it){
		string cmd = it->first + it->second->getOptCmd();
		cmd_list.push_back(cmd);
	}
	vector<string>cmds;
	if(cur_pos <= cmd_end_pos){
		getCmdList(cmd, cmds, cmd_list);
		int n = cmds.size();
		if(n==0){ // Case 4
			mybeep();
		} else if (n == 1){ // Case 3
			string stdcmd = cmds.at(0);
			for(size_t i = cmd.size(); i < stdcmd.size(); ++i){
				insertChar(stdcmd[i]);
			}
			insertChar(' ');
		} else { // Case 2
			for(size_t i = 0; i < cmds.size(); ++i){
				if(!(i % 5)) cout << endl;
					cout << setw(12) << left << cmds.at(i);
			}
			reprintCmd();
		}
	}
	else{
		bool only = getCmdList(cmd, cmds, cmd_list);
		if(only){
			cout << endl;
			getCmd(cmd)->usage(cout);
			reprintCmd();
		}
		else{
			mybeep();
		}
	}
}

// Get Tok plus version

size_t
myStrGetTok2(const string& str, string& tok, size_t pos,
const char del){
	size_t begin = str.find_first_not_of(del, pos);
	if (begin == string::npos) { tok = ""; return begin; }
	size_t end = str.find_first_of(del, begin);
	if(end == string::npos){
		tok = str.substr(begin);
		return str.size();
	}
	tok = str.substr(begin, end - begin);
	return end;
}
// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
   // TODO...
   CmdExec* e = 0;
   for(map<const string, CmdExec*>::iterator it = _cmdMap.begin(); it != _cmdMap.end(); it++){
		if(it->first.find_last_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ")+1 > cmd.size()){ continue; }
		if(!myStrNCmp(cmd, it->first, it->first.find_last_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ")+1)){
			e = it->second;
		}
	}
   return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

