#include "myShell.h"
void myShell::excuteonecommand(parse_info  info,int i,int cmdnum){
  pid_t cpid,w;
  int status;
  std::vector<std::string> para;
  parsecommand(&(info.commandset[i]),&para);
  size_t para_size=para.size();
  size_t envp_size=envp->size();
  //  dealdollar(&para);
  int sel = select(para[0]);
  if(para_size>0 && sel==0){
    size_t ispath = para[0].find("/");//whether we need to use relevant path
    if(ispath==std::string::npos){
      if(search(&para[0])==0){
	std::cout<<"Command "<<para[0]<<" not found"<<std::endl;
      }
    }
    char ** parameter = new char * [para_size+1];
    char ** enviroment = new char * [envp_size+1];
    for(size_t i=0;i<envp_size;i++){
      enviroment[i] = const_cast<char *>((*envp)[i].c_str());
    }
    enviroment[envp_size]=NULL;
    for(size_t i=0;i<para_size;i++){
      parameter[i]=const_cast<char *>(para[i].c_str());
    }
    parameter[para_size]=NULL;
    cpid = fork();
    if(cpid == -1){
      std::cout<<"fork failed"<<std::endl;
      exit(EXIT_FAILURE);
    }
    if(cpid == 0){
      // child process
      int in_fd=-1;
      int out_fd=-1;
      int err_fd=-1;
      if(info.input_index==i){
	in_fd = open((info.inputfile).c_str(),O_RDONLY,0666);
	close(fileno(stdin));
	dup2(in_fd,fileno(stdin));
	close(in_fd);
      }
      if(info.output_index==i){
	out_fd = open((info.outputfile).c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666);
	close(fileno(stdout));
	dup2(out_fd, fileno(stdout));
	close(out_fd);
      }
      if(info.err_index==i){
	err_fd = open((info.errfile).c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666);
	close(fileno(stderr));
	dup2(err_fd, fileno(stderr));
	close(err_fd);
      }
      execve(parameter[0],parameter,enviroment);
      exit(EXIT_SUCCESS);
    }
    else{
      //parent process
      if(i==cmdnum-1){
      do{
	w = waitpid(cpid,&status,WUNTRACED | WCONTINUED);
	if(w == -1){
	  std::cout<<"waitpid wrong"<<std::endl;
	  exit(EXIT_FAILURE);
	}
	if(WIFEXITED(status)){
	  std::cout<<"Program exited with status "<<WEXITSTATUS(status)<<std::endl;
	}
	else if(WIFSIGNALED(status)){
	  std::cout<<"Program was killed by signal "<<WTERMSIG(status)<<std::endl;
	}
	else if(WIFSTOPPED(status)){
	  std::cout<<"Program was stopped by signal "<<WSTOPSIG(status)<<std::endl;
	}
	else if(WIFCONTINUED(status)){
	  std::cout<<"continued"<<std::endl;
	}
      }
      while(!WIFEXITED(status)&& !WIFSIGNALED(status));
    }
    else{
      wait(&status);
    }
    }
    delete[] parameter;
    delete[] enviroment;
  }
  else if(para_size==0){
    std::cout<<"No Command Input"<<std::endl;
  }
  else if(sel!=0){
    switch(sel){
    case 1:{
      if(chdir(para[1].c_str())!=0){
	std::cout<<"change directory failed"<<std::endl;
      }
      break;
    }
    case 2:{
      if(para_size<1){
	std::cout<<"wrong variable numbers for set command"<<std::endl;
      }
      else{
	std::string restcom;
	int i=0;
	while(i<info.command_num){
	  restcom+=info.commandset[i];
	  restcom+=" ";
	  if(info.input_index==i){
	    restcom+="< ";
	    restcom+=info.inputfile;
	    restcom+=" ";
	  }
	  if(info.output_index==i){
	    restcom+="> ";
	    restcom+=info.outputfile;
	    restcom+=" ";
	  }
	  if(info.err_index==i){
	    restcom+="2> ";
	    restcom+=info.errfile;
	    restcom+=" ";
	  }
	  i++;
	  if(i<info.command_num){
	    restcom+="|";
	  }
	}
	size_t index=0;
	std::string space(" ");
	restcom=restcom.substr(4);
	index = restcom.find_first_not_of(space);
	restcom = restcom.substr(index);
	index = restcom.find(space);
	restcom = restcom.substr(index);
	setvar(&para[1],&restcom);
      }
      break;
    }
    case 3:{
      if(para_size!=2){
	std::cout<<"Are you kidding me? It's enviroment variable and you give wrong number of inputs?!!!!!!!!!"<<std::endl;
      }
      else{
	exportvar(&para[1]);
      }
      break;
    }
    default:{
      std::cout<<"Wrong select number for built in command"<<std::endl;
    }
    }
  }
}

void myShell::runpipe(parse_info info){
  int pipe_num=info.command_num-1;
  assert(info.command_num>1);
  int status;
  pid_t pid;
  int pipefds[40];// 20 pipe at most
  for(int i=0;i<pipe_num;i++){
    if(pipe(pipefds+i*2)<0){
      std::cout<<"create pipe failed"<<std::endl;
      exit(EXIT_FAILURE);
    }
  }
  int j=0;
  int ith_command=0;
  while(ith_command<info.command_num){
    pid =fork();
    if(pid==0){
      //if not last command
      if(ith_command<info.command_num-1){
	dup2(pipefds[j+1],fileno(stdout));
      }
      //if not first command
      if(j!=0){
	dup2(pipefds[j-2],fileno(stdin));
      }
      for(int i=0;i<2*pipe_num;i++){
	close(pipefds[i]);
      }
      excuteonecommand(info,ith_command,info.command_num);
      exit(EXIT_SUCCESS);
    }
    else if(pid<0){
      std::cout<<"fork failed"<<std::endl;
    }
    j+=2;
    ith_command++;
  }
  for(int i=0;i<2*(info.command_num-1);i++){
    close(pipefds[i]);
  }
  for(int i=0;i<info.command_num;i++){
    wait(&status);
  }
}

int myShell::zyz(){
  propmt();
  std::string command;
  readcommand(&command);
  trimhead(&command);
  dealdollar(&command);
  if(isend(command.c_str())|| feof(stdin)){
    return 1;
  }
  trimhead(&command);
  if(command.substr(0,3).compare("set")==0){
    if(command.length()==3){
      return 0;
    }
    command=command.substr(4);
    trimhead(&command);
    size_t index = command.find(" ");
    std::string var = command.substr(0,index);
    std::string value;
    if(index==std::string::npos){

    }
    else{
      value = command.substr(index);
      trimhead(&value);
    }
    setvar(&var,&value);
    return 0;
  }
  parse_info info;
  if(!islellgalcommand(&command)){
    std::cout<<"inlellgal command"<<std::endl;
    return 0;
  }
  firstparse(&info,&command);
  if(info.command_num==1){// no pipe
    excuteonecommand(info,0,1);
  }
  else{// have pipe
    runpipe(info);
  }
  return 0;
}

int main(int argc , char** argv){
  std::unordered_map<std::string,std::string> varset;  
  std::vector<std::string> envp;
  myShell yz397(&varset,&envp);    
  yz397.inipath();
  while(1){
    if(yz397.zyz()==1){
      break;
    }
  }
  return EXIT_SUCCESS;
}
