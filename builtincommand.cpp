#include"myShell.h"
int myShell::select(std::string temp){
  std::string cd("cd");
  std::string set("set");
  std::string exp("export");
  if(temp.compare(cd)==0){
    return 1;
  }
  if(temp.compare(set)==0){
    return 2;
  }
  if(temp.compare(exp)==0){
    return 3;
  }
  return 0;
}
bool myShell::islegalvar(std::string * var){
  int i =0;
  int len = var->length();
  while(i<len){
    int temp = (int)var->at(i);
    i++;
    if(temp==95){
      continue;
    }
    if(temp>=65 && temp<=90){
      continue;
    }
    if(temp>=97 && temp<=122){
      continue;
    }
    if(temp>=48 && temp<=57){
      continue;
    }
    return false;
  }
  return true;
}
void myShell::setvar(std::string * var,std::string * value){
  if(var==NULL){
    return;//set _ _    make nothing
  }
  if(!islegalvar(var)){
    std::cout<<"inlegal var name"<<std::endl;
    return ;
  }
  std::unordered_map<std::string,std::string>::const_iterator temp = varset->find(*var);
  if(temp!=varset->end()){
    varset->erase(*var);
  }
  if(value==NULL){
    return ;//set var _        can unset var
  }
  else{
    varset->insert({*var,*value});
  }
}
void myShell::dealdollar(std::string * command){
  std::string dollar("$");
  std::string space(" ");
  size_t start=0;
  size_t end=0;
  std::string ans;
  end = command->find(dollar);
  while(end != std::string::npos){
    ans+=command->substr(start,end-start);
    start =  command->find(space,end);
    std::string temp = command->substr(end+1,start-end-1);
    std::unordered_map<std::string,std::string>::const_iterator it = varset->find(temp);
    if(it!=(varset->end())){
      ans+=it->second;
    }
    end = command->find(dollar,start);
  }
  if(start != std::string::npos){
    ans+=command->substr(start);
  }
  (*command)=ans;
}
void myShell::exportvar(std::string * var){
  std::unordered_map<std::string,std::string>::const_iterator it = varset->find(*var);
  if(it==varset->end()){
    std::cout<<"no such variable"<<std::endl;
  }
  else{
    std::string temp  = it->first;
    for(size_t i=0;i<envp->size();i++){
      if((*envp)[i].substr(0,temp.length()).compare(temp)==0){
	temp = temp+"="+it->second;
	(*envp)[i]=temp;
	return ;
      }
    }
    temp = temp+"="+it->second;
    envp->push_back(temp);
  }
}
