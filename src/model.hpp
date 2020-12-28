#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"
#include "shader.hpp"
#include "math.hpp"
#include "function.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = true);

class Model 
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
	glm::mat4 model_transform;
	string directory;
	bool texGammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const &path = "", 
		glm::mat4 transform = glm::mat4(1.0f),
		bool gamma = true) : 
			texGammaCorrection(gamma), model_transform(transform){
        if(path != "")
			loadModel(path);
	}

	// draws the model, and thus all its meshes
    void Draw(Shader &shader)
    { 
		for(unsigned int i = 0; i < meshes.size(); i++)
             meshes[i].Draw(shader);
	}

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path, Assimp::Importer *importer = NULL, unsigned int pFlags = -1){
        // read file via ASSIMP
		cout << "loading model "<< path << endl;
		if(importer == NULL){
			importer = new Assimp::Importer;
			importer->SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80);	// default ...
		}
		if(pFlags == -1){
			pFlags = aiProcess_Triangulate // all the meshes be converted to tri-mesh.
					 //| aiProcess_GenNormals  		// generate normals if not existing (vertex with face normal, to get average normal use aiProcess_GenSmoothNormals)
					 | aiProcess_GenSmoothNormals	   // generate normals by interpolating face normals, without duplicate vertices
					 | aiProcess_FlipUVs			   // if turned on, we dont have to manually filp textures when read them
					 | aiProcess_CalcTangentSpace	   // Calculate tangent and bitangent vector in model space
					 | aiProcess_PreTransformVertices; // every node / object has a transform matrix, apply it to all vertices in this node;
		}
		const aiScene *scene = importer->ReadFile(path, pFlags);
		// check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {	// if is Not Zero
            cout << "ERROR::ASSIMP:: " << importer->GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));
        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

private:
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene, glm::mat4 prev_transform = glm::mat4(1.0f))
    {
		glm::mat4 transform = Math::to_glm_mat4(node->mTransformation);

		// process each mesh located at the current node
		for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
			processNode(node->mChildren[i], scene, transform * prev_transform);
		}
	}

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }else{
				cout << "this vertex doesent have a normal! \n";
			}
			if(mesh->mColors[0]){
				vertex.Color = glm::vec4(
					mesh->mColors[0][i].r,
					mesh->mColors[0][i].g,
					mesh->mColors[0][i].b,
					mesh->mColors[0][i].a);
			}
			else
				vertex.Color = glm::vec4(1.0);
			// texture coordinates
			if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

		// cout << "NumFaces: " << mesh->mNumFaces <<endl;
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }

		// cout << "MatIndex: " << mesh->mMaterialIndex<<endl;
		if(mesh->mMaterialIndex >= 0){ 			// if mesh has material
			// these materials apply to all vertices in this Mesh node
			// process materials
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
			// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:

			// 1. diffuse maps
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());	
		}
        // return a mesh object created from the extracted mesh data
        Mesh meshr = Mesh(vertices, indices, textures);
		return meshr;
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
		//stbi_set_flip_vertically_on_load(false);
        vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            // A map<str, Texture> may be faster....
			bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
				// for nearly all the textures are created in SRGB space 
				bool with_gamma = (type == aiTextureType::aiTextureType_DIFFUSE) && texGammaCorrection;
				texture.id = TextureFromFile(str.C_Str(), this->directory, with_gamma);
				texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};

// Load a texture, register and bind a GL_TEXTURE_2D
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
	unsigned char *data = loadImage(filename, &width, &height, &nrComponents);
    if (data)
    {
        GLenum format, internel_format;
        if (nrComponents == 1){
            format = GL_RED;
			internel_format = GL_RED;
		}
		else if (nrComponents == 3){
            format = GL_RGB;
			internel_format = gamma ? GL_SRGB : GL_RGB;
		}
		else if (nrComponents == 4){
            format = GL_RGBA;
			internel_format = gamma ? GL_SRGB_ALPHA : GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internel_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
	}

	freeImage(data);
	return textureID;
}