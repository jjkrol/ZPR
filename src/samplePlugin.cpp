#include "../include/samplePlugin.hpp"
#include "../include/params.hpp"
#include "../include/sampleEffect.hpp"
#include <iostream>

using namespace std;
SamplePlugin::SamplePlugin() {
  name = "Sample plugin";
//  box = new Gtk::Box();
 // box->pack_start(button, true, true);
}

Gtk::Widget * SamplePlugin::getWidget(){
  box = new Gtk::Box();
  checkbox = new Gtk::CheckButton("vertical");
  button = new Gtk::ToolButton("asd");
  box->pack_start(*checkbox, false, false);
  box->pack_start(*button, false, false);
  return box;
}

Effect* SamplePlugin::getEffect(Params * params){
if(checkbox==NULL){
    std::cout<<"Probably no widget created!"<<std::endl;
    return new SampleEffect(SampleEffect::NOOPERATION);
  }
if(checkbox->get_active())
  return new SampleEffect(SampleEffect::VERTICAL);
else
  return new SampleEffect(SampleEffect::HORIZONTAL);

}

string SamplePlugin::getName(){
  return name;
}
