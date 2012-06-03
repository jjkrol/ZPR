#pragma once

#include <vector>
#include <string>

class Plugin;

/**
 * Responsible for managing plugins
 */
class PluginManager{
  public:
    PluginManager();
  virtual ~PluginManager(){};
    /**
     * @returns vector of all plugins
     */
    std::vector<Plugin*> getPluginList();

    /**
     * @returns plugin with the specified name
     */
    Plugin * getPluginByName(std::string name);

  private:
    std::vector<Plugin*> plugins;

};
