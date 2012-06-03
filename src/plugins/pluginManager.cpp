#include "../../include/plugins/pluginManager.hpp"
#include "../../include/plugins/plugin.hpp"
#include "../../include/plugins/samplePlugin.hpp"

using namespace std;

PluginManager::PluginManager(){
  //load plugins
  //check for name uniqueness
  plugins.push_back(new SamplePlugin());
}

vector<Plugin*> PluginManager::getPluginList(){
  return plugins;
}

Plugin * PluginManager::getPluginByName(string name){
  vector<Plugin*>::iterator it;
  for(it = plugins.begin(); it != plugins.end(); ++it){
    if((*it)->getName() == name)
      return (*it);
  }
  //return plugins.end();
}