#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "vertex.h"

/*
    output bin format:
        num_vertices : unsigned int
            pos_x pos_y pos_z : float
            norm_x norm_y norm_z : float
            uv_u uv_v : float
            .
            .
            .
        
    tbd
*/
int main(int num_args, char * args[])
{
    if (num_args != 3) {
        std::cout << "Usage: model_convert <input path> <output_path>" << std::endl;
        std::cout << "Example: model_convert input.dae output.bin" << std::endl;
        return 0;
    }

    std::string input_path = args[1];
    std::string output_path = args[2];

    Assimp::Importer importer;
    auto scene = importer.ReadFile(input_path.c_str(),
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
    );

    if (scene) {
        std::vector<graphics::vertex> vertices;

        for (int mesh_idx = 0 ; mesh_idx < scene->mNumMeshes ; mesh_idx++) {
            auto mesh = scene->mMeshes[mesh_idx];
            std::cout << "Mesh: " << std::string(mesh->mName.C_Str()) << std::endl;
            for (int face_idx = 0 ; face_idx < mesh->mNumFaces ; face_idx++) {
                auto face = mesh->mFaces[face_idx];

                for (int index_idx = 0 ; index_idx < face.mNumIndices ; index_idx++) {
                    int idx = face.mIndices[index_idx];

                    graphics::vertex vert;
                    {
                        auto v = mesh->mVertices[idx];
                        vert.pos.x = v.x;
                        vert.pos.y = v.y;
                        vert.pos.z = v.z;
                    }
                    
                    {
                        auto n = mesh->mNormals[idx];
                        vert.normal.x = n.x;
                        vert.normal.y = n.y;
                        vert.normal.z = n.z;
                    }

                    if (mesh->HasTextureCoords(0)) {
                        auto uv = mesh->mTextureCoords[0][idx];
                        vert.uv.x = uv.x;
                        vert.uv.y = uv.y;
                    }

                    vertices.push_back(vert);
                }
            }
            
            for (int bone_idx = 0 ; bone_idx < mesh->mNumBones ; bone_idx++) {
                
            }
        }

        // write out
        std::ofstream file(output_path, std::ofstream::out | std::ofstream::binary );
        if (file.is_open()) {
            std::cout << "Writing to " << output_path << std::endl;
            unsigned int num_vertices = vertices.size();
            file.write((char*)&num_vertices, sizeof(num_vertices));
            for (auto& v : vertices) {
                v.serialise(file);
            }
            file.close();
        }
        else {
            std::cerr << "Unable to write to " << output_path << std::endl;
            return -1;
        }
    }
    else {
        std::cerr << "Unable to load " << input_path << std::endl;
        return -1;
    }

    return 0;
}