#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_
#include <map>
#include <iterator>
#include <vector>
#include <string>
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
// TODO: Add your data members
private:
protected:
  bool forked = false;
  int num_of_args;
  char* args[COMMAND_MAX_ARGS];
public:
  Command(const char* cmd_line = "");
  ~Command();
  virtual void execute() = 0;
  bool isForked() {return forked;}
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class SmallShell 
{
 private:
  // TODO: Add your data members
  std::string prompt;
  char* OLDPWD[COMMAND_ARGS_MAX_LENGTH];
  SmallShell();

 public:
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  void setPrompt(std::string prompt) {this->prompt = prompt;}
  std::string getPrompt() {return this->prompt;}
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};



class BuiltInCommand : public Command {
 private:

 public:
  BuiltInCommand(const char* cmd_line = "");
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
 public:
  ExternalCommand(const char* cmd_line ="");
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line ="");
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChpromptCommand : public BuiltInCommand {
  private:
  std::string name;
  SmallShell* smash;
  public:

  //C'tor D'tor
  ChpromptCommand(const char* cmd_line, SmallShell* smash);
  virtual ~ChpromptCommand() {}
  void execute() override;
};



class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
public:
  std::string newPath;
  char** OLDPWD;
  ChangeDirCommand(const char* cmd_line, char** plastPwd);
  virtual ~ChangeDirCommand() {}
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  char* path;
  GetCurrDirCommand(const char* cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  pid_t pid;
  ShowPidCommand(const char* cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};


enum JOB_STATUS {FOREGROUND,BACKGROUND,STOPPED};

class JobsList {
 public:
  class JobEntry 
  {
   // TODO: Add your data members
    long jobID;
    long jobPID;
    JOB_STATUS status;
    //TODO Add time
  };
 // TODO: Add your data members
 public:
  std::map<long,JobEntry> jobMap;
  JobsList();
  ~JobsList();
  void addJob(Command* cmd, bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs);
  virtual ~JobsCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line, JobsList* jobs);
  virtual ~KillCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class CatCommand : public BuiltInCommand {
 public:
  CatCommand(const char* cmd_line);
  virtual ~CatCommand() {}
  void execute() override;
};



#endif //SMASH_COMMAND_H_
