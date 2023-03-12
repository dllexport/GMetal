#include <MeshExchange.h>

#include <xutility>
#include <iterator>
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Vertex.h>

#include <Geometry.h>

std::vector<IntrusivePtr<Geometry>> processNode(aiNode* node, const aiScene* scene) {
    std::vector<IntrusivePtr<Geometry>> results;

    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        auto geometry = gmetal::make_intrusive<Geometry>();

        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        std::vector<Vertex> vertices;
        std::vector<glm::u32vec3> indices;

        for (int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex v = {};
            auto& meshV = mesh->mVertices[i];
            v.pos = {meshV.x, meshV.y, meshV.z};

            if (mesh->HasNormals()) {
                auto& meshN = mesh->mNormals[i];
                v.normal = {meshN.x, meshN.y, meshN.z};

                if (mesh->HasTangentsAndBitangents()) {
                    auto& meshT = mesh->mTangents[i];
                    v.tangent = {meshT.x, meshT.y, meshT.z, 0.0f};
                }
            }

            if (mesh->HasTextureCoords(0)) {
                auto& meshUV = mesh->mTextureCoords[0][i];
                v.uv = {meshUV.x, meshUV.y};
            }

            if (mesh->HasVertexColors(0)) {
                auto& meshColor = mesh->mColors[i];
                v.color = {meshColor->r, meshColor->g, meshColor->b, meshColor->a};
            }

            vertices.push_back(v);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            if (face.mNumIndices != 3) {
                throw std::runtime_error("face.mNumIndices != 3");
            }
            glm::u32vec3 index = {face.mIndices[0], face.mIndices[1], face.mIndices[2]};
            indices.push_back(index);
        }  

        geometry->vertices = std::move(vertices);
        geometry->indices = std::move(indices);

        results.push_back(geometry);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        auto recursiveResults = processNode(node->mChildren[i], scene);
        std::move(std::begin(recursiveResults),
                  std::end(recursiveResults),
                  std::back_inserter(results));
    }

    return results;
}

IntrusivePtr<Group> MeshExchange::LoadObj(std::string path) { 
	Assimp::Importer assetImporter;
    auto scene = assetImporter.ReadFile(path.c_str(), aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return nullptr;
    }

    auto result = processNode(scene->mRootNode, scene);

    if (result.empty()) {
        return nullptr;
    }

    auto group = gmetal::make_intrusive<Group>();
    std::move(std::begin(result), std::end(result), std::back_inserter(group->children));
    return group;
}
