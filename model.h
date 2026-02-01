#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>

class Model{
    private:
        std::string model="";
        std::vector<std::array<float, 3>> verts;
        std::vector<std::array<int, 3>> faces;

    public:
        Model(std::string model_path);
        void get_file();
};