///currently rubbish
#include "../include/core.hpp"

using namespace boost::filesystem;
using namespace std;

int main(int argc, char* argv[]){

  path p (argv[1]);
  if (!is_directory(p)){
    Photo* myphoto = Photo::initialize(p);
    std::cout<<"Sciezka: "<<myphoto->getPath() << std::endl;
  }
  else{
    Directory* currentDirectory = new Directory(p);
    vector<Photo*> directoryPhotos = currentDirectory->getPhotos();
    for (int i = 0; i < directoryPhotos.size(); i++) {
      std::cout << "Sciezka: "<< directoryPhotos[i]->getPath() << std::endl; 
    }
  }
}


