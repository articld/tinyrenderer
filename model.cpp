#include "model.h"

Model::Model(std::string model_path){
    std::string word ="";
    std::ifstream in(model_path);
    if(!in.good()){
        std::cerr<<"errore";
        return;
    }
    while(in>>word){
       if(word == "v")
            break;
    }
    in.close();
}

void Model::get_file(){
    std::cout<<model;
}