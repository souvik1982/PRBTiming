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

void readConfigurationFile(std::string schematicFilename, MapComponentRelations *map_componentRelations)
{
  std::cout<<"schematicFilename = "<<schematicFilename<<std::endl;
  std::ifstream file(schematicFilename.c_str());
  std::string s;
  getline(file, s);
  while (!file.eof())
  {
    getline(file, s);
    removeSpaces(s);
    
    if (s=="") continue; // This is an empty line
    if (s.at(0)=='#') continue; // This is a comment
    
    if (s.find("Import")!=std::string::npos) // Go into this schematic
    {
      std::string subSchematicFilename=argInLine(s, "Import");
      readConfigurationFile(subSchematicFilename, map_componentRelations);
    }
    else if (s.find("Index")!=std::string::npos)
    {
      int index=atoi(argInLine(s, "Index").c_str());
      if (map_componentRelations->find(index)==map_componentRelations->end())
      {
        getline(file, s);
        std::string componentType=argInLine(s, "ComponentType");
        
        if (componentType=="CIC")
        {
          std::map<std::string, std::string> map_params=readConfigurationLine(&file, s);
          std::string name_s;  if (map_params.find("ComponentName")!=map_params.end()) name_s=map_params["ComponentName"]; else std::cout<<"ERROR: CIC ComponentName does not exist in configuration file"<<std::endl;
          int moduleID;        if (map_params.find("ModuleID")!=map_params.end()) moduleID=atoi(map_params["ModuleID"].c_str()); else std::cout<<"ERROR: CIC ModuleID does not exist in configuration file"<<std::endl;
          std::string segment; if (map_params.find("Segment")!=map_params.end()) segment=map_params["Segment"]; else std::cout<<"ERROR: CIC Segment does not exist in configuration file"<<std::endl;
          double frequency;    if (map_params.find("Frequency")!=map_params.end()) frequency=atof(map_params["Frequency"].c_str()); else std::cout<<"ERROR: CIC Frequency does not exist in configuration file"<<std::endl;
          CIC *cic=new CIC(name_s, moduleID, segment, frequency);
          ComponentRelation *compRelation=new ComponentRelation();
          while (s!="")
          {
            std::string outputConnections=argInLine(s, "OutputConnections");
            compRelation->extractComponentRelation(outputConnections);
            getline(file, s);
          }
          compRelation->comp_=cic;
          (*map_componentRelations)[index]=compRelation;
        }
        
        if (componentType=="Receiver")
        {
          std::map<std::string, std::string> map_params=readConfigurationLine(&file, s);
          std::string name_s;  if (map_params.find("ComponentName")!=map_params.end()) name_s=map_params["ComponentName"]; else std::cout<<"ERROR: CIC ComponentName does not exist in configuration file"<<std::endl;
          double frequency;    if (map_params.find("Frequency")!=map_params.end()) frequency=atof(map_params["Frequency"].c_str()); else std::cout<<"ERROR: CIC Frequency does not exist in configuration file"<<std::endl;
          double delayCLK;     if (map_params.find("DelayCLK")!=map_params.end()) delayCLK=atof(map_params["DelayCLK"].c_str()); else std::cout<<"ERROR: CIC DelayCLK does not exist in configuration file"<<std::endl;
          Receiver *receiver=new Receiver(name_s, frequency, delayCLK);
          ComponentRelation *compRelation=new ComponentRelation();
          while (s!="")
          {
            std::string outputConnections=argInLine(s, "OutputConnections");
            compRelation->extractComponentRelation(outputConnections);
            getline(file, s);
          }
          compRelation->comp_=receiver;
          (*map_componentRelations)[index]=compRelation;
        }
        
        if (componentType=="BXSplitter")
        {
          std::map<std::string, std::string> map_params=readConfigurationLine(&file, s);
          std::string name_s;  if (map_params.find("ComponentName")!=map_params.end()) name_s=map_params["ComponentName"]; else std::cout<<"ERROR: CIC ComponentName does not exist in configuration file"<<std::endl;
          double frequency;    if (map_params.find("Frequency")!=map_params.end()) frequency=atof(map_params["Frequency"].c_str()); else std::cout<<"ERROR: CIC Frequency does not exist in configuration file"<<std::endl;
          BXSplitter *bxSplitter=new BXSplitter(name_s, frequency);
          ComponentRelation *compRelation=new ComponentRelation();
          while (s!="")
          {
            std::string outputConnections=argInLine(s, "OutputConnections");
            compRelation->extractComponentRelation(outputConnections);
            getline(file, s);
          }
          compRelation->comp_=bxSplitter;
          (*map_componentRelations)[index]=compRelation;
        }
        
        if (componentType=="LayerSplitter")
        {
          std::map<std::string, std::string> map_params=readConfigurationLine(&file, s);
          std::string name_s;  if (map_params.find("ComponentName")!=map_params.end()) name_s=map_params["ComponentName"]; else std::cout<<"ERROR: CIC ComponentName does not exist in configuration file"<<std::endl;
          double frequency;    if (map_params.find("Frequency")!=map_params.end()) frequency=atof(map_params["Frequency"].c_str()); else std::cout<<"ERROR: CIC Frequency does not exist in configuration file"<<std::endl;
          LayerSplitter *layerSplitter=new LayerSplitter(name_s, frequency);
          ComponentRelation *compRelation=new ComponentRelation();
          while (s!="")
          {
            std::string outputConnections=argInLine(s, "OutputConnections");
            compRelation->extractComponentRelation(outputConnections);
            getline(file, s);
          }
          compRelation->comp_=layerSplitter;
          (*map_componentRelations)[index]=compRelation;
        }
        
        
      }
      else
      {
        std::cout<<"WARNING: Index of this component "<<index<<" already exists."<<std::endl;
      }
    } // Found new Index
  } // Done reading schematic file
}
