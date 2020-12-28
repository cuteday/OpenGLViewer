/*
 * Deal with diffrent formats or datasets' 3D scene loading work
 */
#pragma once

#include <string>
#include <scene.hpp>
#include <assimp/Importer.hpp>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/filesystem.hpp>

namespace Importer{
	/*
	 *	This function loads a house in 3D-FRONT dataset
	 * 	The house model should be prosessed by json2obj.py in 3D-FRONT-Toolbox
	 */
	using namespace std;
	using namespace cv::utils;

	Scene* Import3DFront(string path){
		if(!fs::exists(path) || !fs::isDirectory(path)){
			cout << "3D-Front house directory path "<< path <<" not exist or not a directory!\n";
			return NULL;
		}
		Scene* scene = new Scene();
		vector<string> rooms;
		fs::glob(path, "", rooms, false, true);
		for(string room:rooms){
			if(!fs::isDirectory(room))
				continue;
			vector<string> objs;
			fs::glob(room, "*.obj", objs, false, true);
			for(string obj: objs){
				Assimp::Importer *importer = NULL;
				if (obj.find("mesh.obj") != string::npos ){
					importer = new Assimp::Importer;
					importer->SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80);
				}
				Model model;
				model.loadModel(obj, importer);
				scene->models.push_back(model);
			}
		}
		return scene;
	}

	// Scene* Import3DFront(string path_str){
	// 	std::filesystem::path path_base(path_str);
	// 	if(!filesystem::exists(path_base)){
	// 		cout << "3D-Front house directory path "<< path_base.string() <<" not exist!\n";
	// 		return NULL;
	// 	}
	// 	std::filesystem::directory_entry dir(path_base);
	// 	if(!dir.is_directory()){
	// 		cout << "Not a directory...\n";
	// 		return NULL;
	// 	}
	// 	Scene* scene = new Scene();
	// 	std::filesystem::directory_iterator dir_list(path_base);
	// 	for(auto& room : dir_list){
	// 		if(!room.is_directory())
	// 			continue;
	// 		std::filesystem::directory_iterator obj_list(room);
	// 		for(auto obj : obj_list){
	// 			if(!obj.is_regular_file())
	// 				continue;
	// 			if(obj.path().string().find(".obj") != string::npos){
	// 				// we load every wavefront .obj files 
	// 				// each obj file is a furniture or a room layout
	// 				Assimp::Importer *importer = NULL;
	// 				if(obj.path().filename().string() == "mesh.obj"){
	// 					importer = new Assimp::Importer;
	// 					importer->SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80);
	// 				}
	// 				Model model;
	// 				model.loadModel(obj.path().string(), importer);
	// 				scene->models.push_back(model);
	// 			}
	// 		}
	// 	}
	// 	return scene;
	// }
}