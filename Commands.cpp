#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"

using namespace std;

#define WHITESPACE " "
#define BACKSLASH "/"

/*
* Macro providing a “safe” way to invoke system calls
*/
#define DO_SYS( syscall ) do { \
/* safely invoke a system call */ \
if( (syscall) == -1 ) { \
perror( #syscall ); \
exit(1); \
} \
} while( 0 )

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

string _ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
  return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h 

//---------------------------------------------------_____Smalll Shell_____---------------------------------------//
SmallShell::SmallShell() {
// TODO: add your implementation
  prompt = "smash> ";
  *OLDPWD = NULL;
}

SmallShell::~SmallShell() {
// TODO: add your implementation


}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
	// For example:

string cmd_s = _trim(string(cmd_line));
string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

if(firstWord.compare("chprompt") == 0)
{
  return new ChpromptCommand(cmd_line, this);
}
else if(firstWord.compare("showpid") == 0)
{
  return new ShowPidCommand(cmd_line);
}
else if (firstWord.compare("pwd") == 0)
{
  return new GetCurrDirCommand(cmd_line);
}
else if (firstWord.compare("cd") == 0)
{
  return new ChangeDirCommand(cmd_line,this->OLDPWD);
}
else if (firstWord.compare("quit") == 0)
{
  return new QuitCommand(cmd_line,&this->jobs,getpid());
}
return nullptr;
}


void SmallShell::executeCommand(const char *cmd_line) {
  // TODO: Add your implementation here
  // for example:
  // Command* cmd = CreateCommand(cmd_line);
  // cmd->execute();
  // Please note that you must fork smash process for some commands (e.g., external commands....)
  Command* cmd = CreateCommand(cmd_line);
  if(cmd != nullptr)
  {
   if(cmd->isForked() == true)
   {
     //fork process
   }
   else
   {
     //no fork
     cmd->execute();
   }
  }
   
} 


//----Command
//C'tor
Command::Command(const char* cmd_line)
{
  if(_isBackgroundComamnd(cmd_line))
  {
    background = true;
    
  }
  string clean_cmd(cmd_line);
  clean_cmd = clean_cmd.substr(0,clean_cmd.find_last_not_of(WHITESPACE));
  num_of_args = _parseCommandLine(clean_cmd.c_str(), args);
}

//D'tor
Command::~Command()
{
  for (int i = 0; i< COMMAND_MAX_ARGS; i++)
  {
    free (args[i]);
  }
}


//----BuiltinCommand
//C'tor
BuiltInCommand::BuiltInCommand(const char* cmd_line)
:Command(cmd_line)
{
  this->forked = false;
}

//----ExternalinCommand
//C'tor



//----PipeCommand
//----RederectionCommand
//----ChpromptCommand
//C'tor
ChpromptCommand::ChpromptCommand(const char* cmd_line, SmallShell* smash)
:BuiltInCommand(cmd_line)
{
  this->smash = smash;
  
  if(this->args[1] == NULL)
  {
    this->name = "smash> ";
  }
  else
  {
    this->name = this->args[1];
    this->name.append("> ");
  }
}
//D'tor
//Execute
void ChpromptCommand::execute()
{
  //cout<<this->name;
  this->smash->setPrompt(this->name);
  //cout<<this->smash->getPrompt();
}

//----ShowPidCommand
//C'tor
ShowPidCommand::ShowPidCommand(const char* cmd_line)
:BuiltInCommand(cmd_line)
{
  this->pid = getpid();
  
}

void ShowPidCommand::execute()
{
  std::cout << "smash pid is " << this->pid << std::endl;
}

//----GetCurrDirCommand
//C'tor
GetCurrDirCommand::GetCurrDirCommand(const char* cmd_line)
:BuiltInCommand(cmd_line)
{
  path = getcwd(path, COMMAND_ARGS_MAX_LENGTH);
}

void GetCurrDirCommand::execute()
{
  cout<<path<<endl;
}

//-----ChangeDirCommand

ChangeDirCommand::ChangeDirCommand(const char* cmd_line, char** plastPwd)
:BuiltInCommand(cmd_line)
{
  if(num_of_args != 1)
  {
  this->newPath = args[1];
  }
  else
  {
    this->newPath = "";
  }
  this->OLDPWD = plastPwd;
}

void ChangeDirCommand::execute()
{
  if(num_of_args > 2)
  { 
    //TODO ERROR too many args
  }
  else
  {
    char* currpath = NULL;
    currpath = getcwd(currpath, COMMAND_ARGS_MAX_LENGTH); 
    if(newPath.compare("-") == 0)
    {
      if(*(this->OLDPWD) == NULL)
      {
        //TODO ERROR OLDPWD NOT SET
      }
      else
      {
        //Change newpath to old DIr
        this->newPath = *(this->OLDPWD);
      }
    }
    else if(newPath.compare("..") == 0)
    {
      std::string curr = currpath;
      this->newPath = curr.substr(0,curr.find_last_of('/'));
    }
    if(chdir(newPath.c_str()) != -1)
    {
     *(this->OLDPWD) = currpath;
    }
    else
    {
    //TODO ERROR Syserror 
    std::cout<<"path failed TODO"<<std::endl;
    }
  }
}


//------QuitCommand

QuitCommand::QuitCommand(const char* cmd_line, JobsList* jobs,pid_t smash_pid)
:BuiltInCommand(cmd_line),
smash_pid(smash_pid),
jobs(jobs)
{}

void QuitCommand::execute()
{
  if(num_of_args >> 1)
  {
    std::string arg = args[1];
    if(arg.compare("kill") == 0)
    {
      //kill all jobs
      std::cout<<"smash: sending SIGKILL signal to "<< jobs->jobsMap.size() << "jobs:" << std::endl;
      for (auto itr = jobs->jobsMap.begin();itr != jobs->jobsMap.end(); itr++)
      {
        std::cout<<itr->second.jobPID << ": " << itr->second.cmd_line << std::endl;
      }
      jobs->killAllJobs();
    }
  }
  kill(smash_pid,9);
}


//----- KILLALLJOBS !

void JobsList::killAllJobs()
{
  for (auto itr = jobsMap.begin();itr != jobsMap.end(); itr++)
  {
    kill(itr->second.jobPID,9);
  }
}