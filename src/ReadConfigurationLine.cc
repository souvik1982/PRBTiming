#include "string"
#include "map"

std::string argInLine(std::string line, std::string key)
{
  removeSpaces(line);
  std::size_t pos=line.find(key);
  if (pos==std::string::npos)
  {
    return "";
  }
  else
  {
    return line.substr(key.size()+1);
  }
}

std::map<std::string, std::string> readConfigurationLine(std::ifstream *file, std::string &line)
{
  std::map<std::string, std::string> configMap;
  getline(*file, line);
  while (line.find("OutputConnections")==std::string::npos)
  {
    removeSpaces(line);
    std::size_t pos=line.find(":");
    if (pos!=std::string::npos)
    {
      std::string key=line.substr(0,pos);
      std::string value=line.substr(pos+1, line.size());
      // std::cout<<"key = "<<key<<", value = "<<value<<std::endl;
      configMap[key]=value;
    }
    getline(*file, line);
  }
  
  return configMap;
}
