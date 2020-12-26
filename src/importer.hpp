/*
 * TO DO: 
 * Deal with diffrent formats or datasets' 3D scene loading work
 */

#pragma once

#include <filesystem>
#include <string>

#include <scene.hpp>

using namespace std;
using namespace std::filesystem;

namespace Importer{

Scene* Import3DFront(string path_str){
	filesystem::path path_base(path_str);
	if(!filesystem::exists(path_base)){
		cout << "3D-Front house directory path"<< path_base.string() <<" not exist!\n";
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
				Model model(obj.path().string());
				scene->models.push_back(model);
			}
		}
	}
	return scene;
}
}