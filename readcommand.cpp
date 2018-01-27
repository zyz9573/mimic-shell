#include"myShell.h"
bool myShell::isend(const char * input){
  std::string s(input);
  for(size_t i=0;i<s.length();i++){
    if((int)input[i]== -1){return true;}
  }
  int start = s.find_first_not_of(" \t");
  int end = s.find_last_not_of(" \t");
  if((end-start) != 3){
    return false;
  }
  else{
    if(s.at(start)=='e'){
      if(s.at(start+1)=='x'){
	if(s.at(start+2)=='i'){
	  if(s.at(start+3)=='t'){
	    return true;
	  }
	}
      }
    }
  }
  return false;
}
void myShell::readcommand(std::string * command){
    std::getline(std::cin,(*command));
}
void myShell::dealbslash(std::string * s){
  size_t index=0;
  std::string bslash("\\");
  index=(*s).find(bslash);
  while(index!=std::string::npos){
    if(index!=(*s).length()-1){
      if((*s).at(index+1)==' '){
	(*s).erase(index,1);
	if(index>0){
	  index--;
	}
      }
    }
    index = (*s).find(bslash,index+1);
  }
}
void myShell::parsecommand(std::string * command,std::vector<std::string> * para){
  std::string space(" ");
  trimhead(command);
  size_t start=0;
  size_t mid=0;
  size_t end = (*command).find(space);
  while(std::string::npos!=end){
    if((*command).at(end-1)!=92){
      if(end==start){
	start=end+1;
      }
      else{
	std::string temp = (*command).substr(start,end-start);
	dealbslash(&temp);
	(*para).push_back(temp);
	start=end+1;
      }
    }
    mid=end+1;
    end=(*command).find(space,mid);
  }
  if(start!=(*command).length()){
    std::string temp = (*command).substr(start);
    dealbslash(&temp);
    (*para).push_back(temp);
  }
}
void myShell::trimhead(std::string * command){
  size_t index = command->find_first_not_of(" \t");
  if(index!=std::string::npos){
    (*command) = command->substr(index);
  }
}
void myShell::trimtail(std::string * command){
  size_t index = command->find_last_not_of(" \t");
  if(index!=std::string::npos){
    (*command) = command->substr(0,index+1);
  }
}
void myShell::firstparse(parse_info * info,std::string * command){
  size_t start = 0;
  size_t end = 0;
  std::string pipe("|");
  std::string redin("<");
  std::string redout(" >");
  std::string rederr("2>");
  end = command->find(pipe);
  while(end!=std::string::npos){
    (info->commandset).push_back(command->substr(start,end-start));
    start = end+1;
    end = command->find(pipe,start);
  }
  if(start != command->length()){
    std::string temp = command->substr(start);
    (info->commandset).push_back(temp);
  }
  info->command_num = info->commandset.size();
  size_t in=0;
  size_t out=0;
  size_t err=0;
  for(size_t i =0;i<info->commandset.size();i++){
    in = (info->commandset)[i].find(redin);
    out = (info->commandset)[i].find(redout);
    err = (info->commandset)[i].find(rederr);
    std::string temp = info->commandset[i];
    if(in!=std::string::npos){
      if(out!=std::string::npos){
	if(err!=std::string::npos){
	  (info->commandset)[i] = temp.substr(0,in);
	  info->inputfile = temp.substr(in+1,out-in-1);
	  info->outputfile = temp.substr(out+2,err-out-2);
	  info->errfile = temp.substr(err+2);
	  info->input_index = i;
	  info->output_index = i;
	  info->err_index = i;
	}
	else{
	  (info->commandset)[i] = temp.substr(0,in);
	  info->inputfile = temp.substr(in+1,out-in-1);
	  info->outputfile = temp.substr(out+2);
	  info->input_index = i;
	  info->output_index = i;
	}
      }
      else{
	if(err!=std::string::npos){
	  (info->commandset)[i] = temp.substr(0,in);
	  info->inputfile = temp.substr(in+1,err-in-1);
	  info->errfile = temp.substr(err+2);
	  info->input_index = i;
	  info->err_index = i;
	}
	else{
	  (info->commandset)[i] = temp.substr(0,in);
	  info->inputfile = temp.substr(in+1);
	  info->input_index = i;
	}
      }
    }
    else{
      if(out!=std::string::npos){
	if(err!=std::string::npos){
	  (info->commandset)[i] = temp.substr(0,out);
	  info->outputfile = temp.substr(out+2,err-out-2);
	  info->errfile = temp.substr(err+2);
	  info->output_index = i;
	  info->err_index = i;
	}
	else{
	  (info->commandset)[i] = temp.substr(0,out);
	  info->outputfile = temp.substr(out+2);
	  info->output_index = i;
	}
      }
      else{
	if(err!=std::string::npos){
	  (info->commandset)[i] = temp.substr(0,err);
	  info->errfile = temp.substr(err+2);
	  info->err_index = i;
	}
      }
    }
  }
  trimhead(&(info->inputfile));
  trimhead(&(info->outputfile));
  trimhead(&(info->errfile));
  trimtail(&(info->inputfile));
  trimtail(&(info->outputfile));
  trimtail(&(info->errfile));
}
bool myShell::islellgalcommand(std::string * command){
  std::string redin("<");
  std::string redout(">");
  std::string rederr("2>");
  size_t in = command->find(redin);
  size_t out = command->find(redout);
  size_t err = command->find(rederr);
  if(in!=std::string::npos){
    if(command->find_first_of(redin)!=command->find_last_of(redin)){return false;}
  }
  else{in=0;}
  if(out!=std::string::npos){
    if(err == std::string::npos){
      if(command->find_first_of(redout)!=command->find_last_of(redout)){return false;}
    }
    else{
      if(command->find_first_of(redout)!=command->find_last_of(redout,err)){return false;}
    }
  }
  if(err != std::string::npos){
    size_t temp = command->find(err+1);
    if(temp!=std::string::npos){return false;}
  }
  if(in>out || in>err){return false;}
  if(out>err){return false;}
  return true;
}
