#include"myShell.h"
void myShell::inipath(){
  std::string path;
  getpath(&path);
  varset->insert({"PATH",path});
  std::string temp("PATH=");
  path = temp+path;
  envp->push_back(path);
}
int myShell::getpath(std::string * path){
  char * p;
  if((p=getenv("PATH"))){
    if(p==NULL){return 0;}
    std::string temp(p);
    *path = temp;
    return 1;
  }
  return 0;
}
void myShell::dividepath(std::string s, std::vector<std::string> * ans){
  std::string colon(":");
  size_t start=0;
  size_t end = s.find(colon);
  while(std::string::npos!=end){
    (*ans).push_back(s.substr(start,end-start));
    start = end+1;
    end  = s.find(colon,start);
  }
  if(start!=s.length()){
    (*ans).push_back(s.substr(start));
  }
}
int myShell::searchcommand(std::vector<std::string> * path,std::string * filename){
  DIR * dir;
  int sign=0;
  struct dirent * ptr = NULL;
  while(!path->empty()){
    if(sign==0){
      std::string nd = (*path).back();
      dir = opendir(nd.c_str());
      (*path).pop_back();
      while((ptr = readdir(dir))!=NULL){
	std::string temp(ptr->d_name);
	if((*filename).compare(temp)==0){
	  nd = nd +"/"+temp;
	  *filename = nd;
	  sign=1;
	}
      }
      free(dir);
    }
    else{(*path).pop_back();}
  }
  return sign;
}
int myShell::search(std::string * command){
  std::string path;
  std::vector<std::string> temp;
  std::string prefix("PATH=");
  int sign=0;
  for(size_t i=0;i<envp->size();i++){
    if((*envp)[i].substr(0,5).compare(prefix)==0){
      path = (*envp)[i].substr(5);
      break;
    }
  }
  if(path.length()>0){
    dividepath(path,&temp);
    sign = searchcommand(&temp,command);
  }
  else{
    std::cout<<"get path failed"<<std::endl;
  }
  return sign;
}
