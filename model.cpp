#include "model.h"

Model::Model(std::string model_path){
    std::array<float, 3> vert_temp;
    std::string type="";
    std::ifstream in(model_path);

    if(!in.good()){
        std::cerr<<"path non corretto";
        return;
    }

    while(!in.eof()){
        std::string line;
        std::string trash;

        std::getline(in, line);
        std::istringstream iss(line.c_str());

        if(!line.compare(0, 2, "v ")){
            iss >> trash;
            for(int i : {0, 1, 2}) iss >> vert_temp[i];
            verts.push_back(vert_temp);
        }

        else if(!line.compare(0,2, "f ")){
            int f, t, n, count = 0;
            iss >> trash;
            while(iss>> f >> trash >> t >> trash >> n){
                faces.push_back(--f);
                count ++;
            }
            if (count != 3){
                std::cerr<<"faccia degenere";
                return;
            }
        }
    }

    auto iter = verts.begin();
    for(; iter!=verts.end(); iter++){
        for(int i = 0; i<3; i++){
            std::cout<<(*iter)[i] <<std::endl;
        }
    }

    auto iter1 = faces.begin();
    for(; iter1 != faces.end(); iter++){
        for(int i = 0; i<3; i++){
            std::cout<<(*iter)[i] <<std::endl;
        }
    }

    in.close();
}

void Model::get_file(){
    std::cout<<model;
}