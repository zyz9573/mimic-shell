#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<cstdlib>
#include<cstdio>
#include<iostream>
#include<string.h>
#include<string>
#include<sys/wait.h>
#include<sys/types.h>
#include<assert.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<pwd.h>
#include<signal.h>
#include<vector>
#include<dirent.h>
#include<unordered_map>
struct parse_info{
  std::vector<std::string> commandset;
  std::string  inputfile;
  std::string  outputfile;
  std::string  errfile;
  int input_index=-1;
  int output_index=-1;
  int err_index=-1;
  int command_num=0;
};
class myShell{
 private:
  std::unordered_map<std::string,std::string> * varset;
  std::vector<std::string> * envp;
 public:
  myShell(std::unordered_map<std::string,std::string> * a , std::vector<std::string> * b){
    varset = a;
    envp = b;
  }
  int zyz();//main function for this class, return 1 if something is wrong ,else return 0
  void excuteonecommand(parse_info info, int i,int cmdnum);// excute a single command without pipe
  void runpipe(parse_info info);// excute command with pipe
  void propmt(){
    char pathname[1024];
    getcwd(pathname,1024);
    std::cout<<"myShell:"<<pathname<<" $";
  }
  //define in searchpath.cpp
  void inipath();//get system PATH and put it into myShell's environment variable
  int getpath(std::string * path);//get current environment path for myShell,return 0 if success, else return 1
  void dividepath(std::string s, std::vector<std::string> * ans);//divide path based on :  ,store  divided path into a vector
  int searchcommand(std::vector<std::string> * path,std::string * filename);//search command based on path in stored vector, return 0 if success , else return 1
  int search(std::string * command);//search command
  //define in readcommand.cpp
  bool isend(const char * input);//return true if input mean end
  void readcommand(std::string * command);//read command from stdin
  void dealbslash(std::string * s);//deal with /
  void parsecommand(std::string * command,std::vector<std::string> * para);//parse a single command like   ./hello a b c
  void trimhead(std::string * command);//remove spaces at start
  void trimtail(std::string * command);//remove spaces at end
  void firstparse(parse_info * info,std::string * command);//parse complicated command like a < b | c > d | e 2> f into several single command and store relevant information
  bool islellgalcommand(std::string * command);//return false if input command violate setted rules 
  //define in builtincommand.cpp
  bool islegalvar(std::string * var);//return false if a variable name is wrong
  int select(std::string temp);//select which command to excute
  void setvar(std::string * var,std::string * value);//set variable
  void dealdollar(std::string * command);//deal with $
  void exportvar(std::string * var);//export variable to environment
};
