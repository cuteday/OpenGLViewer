/*
 * TO DO: 
 * Deal with diffrent formats or datasets' 3D scene loading work
 */

#pragma once

#include <filesystem>
#include <string>
#include <assimp/Importer.hpp>
#include <scene.hpp>

using namespace std;
using namespace std::filesystem;

namespace Importer{
	/*
	 *	This function loads a house in 3D-FRONT dataset
	 * 	The house model should be prosessed by json2obj.py in 3D-FRONT-Toolbox
	 */
	Scene* Import3DFront(string path_str){
		filesystem::path path_base(path_str);
		if(!filesystem::exists(path_base)){
			cout << "3D-Front house directory path "<< path_base.string() <<" not exist!\n";
			return NULL;
		}
		directory_entry dir(path_base);
		if(!dir.is_directory()){
			cout << "Not a directory...\n";
			return NULL;
		}
		Scene* scene = new Scene();
		directory_iterator dir_list(path_base);
		for(auto& room : dir_list){
			if(!room.is_directory())
				continue;
			directory_iterator obj_list(room);
			for(auto obj : obj_list){
				if(!obj.is_regular_file())
					continue;
				if(obj.path().string().find(".obj") != string::npos){
					// we load every wavefront .obj files 
					// each obj file is a furniture or a room layout
					Assimp::Importer *importer = NULL;
					if(obj.path().filename().string() == "mesh.obj"){
						importer = new Assimp::Importer;
						importer->SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80);
					}
					Model model;
					model.loadModel(obj.path().string(), importer);
					scene->models.push_back(model);
				}
			}
		}
		return scene;
	}
}