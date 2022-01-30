#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "vertex.h"

struct vert_bone_pair
{
    int bone_idx;
    float weight;
};

struct bone_parent_pair
{
    int bone_idx;
    int parent_idx = -1;
};

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
        std::vector<graphics::bone> bones;
        std::map<int,std::vector<vert_bone_pair>> vert_to_bone_map;
        std::map<std::string,int> bone_name_to_idx;
        std::vector<bone_parent_pair> bone_hierarchy_info;

        if (scene->mNumMeshes != 1) {
            std::cerr << "Only supports 1 mesh at the moment\n";
            return -1;
        }

        for (int mesh_idx = 0 ; mesh_idx < scene->mNumMeshes ; mesh_idx++) {
            auto mesh = scene->mMeshes[mesh_idx];
            // do bones first as for simplicity we drop indices so it'll be more work to associate the bones later
            std::cout << "Exporting bones:\n";
            for (int bone_idx = 0 ; bone_idx < mesh->mNumBones ; bone_idx++) {
                auto bone = mesh->mBones[bone_idx];
                std::cout << "Bone: " << std::string(bone->mName.C_Str()) << std::endl;
                auto offset_matrix = bone->mOffsetMatrix;

                graphics::bone bone_data;
                for (int row = 0; row < 4; row++) {
                    for (int col = 0; col < 4; col++) {
                        bone_data.offset_matrix[col][row] = offset_matrix[col][row];
                    }
                }

                bone_name_to_idx[std::string(bone->mName.C_Str())] = bone_idx;
                bones.push_back(bone_data);

                for (int weight_idx = 0 ; weight_idx < bone->mNumWeights ; weight_idx++) {
                    auto weight = bone->mWeights[weight_idx];
                    
                    vert_bone_pair pair;
                    pair.bone_idx = bone_idx;
                    pair.weight = weight.mWeight;

                    vert_to_bone_map[weight.mVertexId].push_back(pair);
                }
            }

            for (auto bone_entry : bone_name_to_idx) {
                auto bone_node = scene->mRootNode->FindNode(bone_entry.first.c_str());
                if (bone_node) {
                    std::string current_node_name(bone_node->mName.C_Str());
                    std::string parent_node_name(bone_node->mParent->mName.C_Str());

                    bone_parent_pair pair;
                    pair.bone_idx = bone_name_to_idx[current_node_name];

                    auto parent_iter = bone_name_to_idx.find(parent_node_name);
                    if (parent_iter != bone_name_to_idx.end()) {
                        pair.parent_idx = parent_iter->second;
                    }
                    bone_hierarchy_info.push_back(pair);
                }
                else {
                    std::cerr << "Can't find bone: " << bone_entry.first << std::endl;
                    return -1;
                }
            }

            std::cout << "Exporting meshes:\n";
            std::cout << "Mesh: " << std::string(mesh->mName.C_Str()) << std::endl;
            for (int face_idx = 0 ; face_idx < mesh->mNumFaces ; face_idx++) {
                auto face = mesh->mFaces[face_idx];

                for (int index_idx = 0 ; index_idx < face.mNumIndices ; index_idx++) {
                    int vert_idx = face.mIndices[index_idx];

                    graphics::vertex vert;
                    {
                        auto v = mesh->mVertices[vert_idx];
                        vert.pos = {v.x, v.y, v.z};
                    }
                    
                    {
                        auto n = mesh->mNormals[vert_idx];
                        vert.normal = {n.x, n.y, n.z};
                    }

                    if (mesh->HasTextureCoords(0)) {
                        auto uv = mesh->mTextureCoords[0][vert_idx];
                        vert.uv = {uv.x, uv.y};
                    }

                    auto vert_to_bone_entry = vert_to_bone_map.find(vert_idx);
                    if (vert_to_bone_entry != vert_to_bone_map.end()) {
                        int num_bones = vert_to_bone_entry->second.size();
                        if (num_bones > 4) {
                            std::cerr << "More than 4 bones for a vertex, only using first 4\n";
                            num_bones = 4;
                        }
                        for (int bone_idx = 0; bone_idx < num_bones ; bone_idx++) {
                            auto bone = vert_to_bone_entry->second[bone_idx];
                            vert.bone_ids[bone_idx] = bone.bone_idx;
                            vert.bone_weights[bone_idx] = bone.weight;
                        }
                    }

                    vertices.push_back(vert);
                }
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