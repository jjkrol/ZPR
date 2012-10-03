#include "../include/plugins/pluginManager.hpp"
#include "../include/plugins/plugin.hpp"
#include "../include/plugins/samplePlugin.hpp"
#include "../include/plugins/desaturatePlugin.hpp"

using namespace std;

PluginManager::PluginManager() {
  plugins.push_back(new SamplePlugin());
  plugins.push_back(new DesaturatePlugin());
}

vector<Plugin*> PluginManager::getPluginList() {
  return plugins;
}

Plugin* PluginManager::getPluginByName(string name) {
  vector<Plugin*>::iterator it;
  for(it = plugins.begin(); it != plugins.end(); ++it){
    if((*it)->getName() == name)
      return (*it);
  }
  return NULL;
}
