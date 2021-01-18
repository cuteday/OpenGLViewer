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
		if(path == "" || !fs::exists(path) || !fs::isDirectory(path)){
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
				unsigned int pFlags = -1;
				if (obj.find("mesh.obj") != string::npos){
					importer = new Assimp::Importer;
					pFlags = defaultPFlags | aiProcess_ConvertToLeftHanded;
					importer->SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80);
				}
				Model model;
				model.loadModel(obj, importer, pFlags);
				scene->models.push_back(model);
			}
		}
		return scene;
	}

}