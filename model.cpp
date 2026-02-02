#include "model.h"

Model::Model(std::string model_path){
    vec3 vert_temp;
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
            //TODO: aggiungere il check per verificare se le facce sono triangolari
            int f, t, n;
            iss >> trash;
            while(iss>> f >> trash >> t >> trash >> n){
                std::cout<<f<<std::endl;
                faces.push_back(--f);
            }
        }

    }
    in.close();
}
