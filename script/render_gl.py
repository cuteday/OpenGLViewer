import os

path_renderer = os.path.expanduser("./build/bin/cam2img")
path_3dfront = os.path.expanduser("~/Public/Projects/OpenGLViewer/model/3dfront")
path_renders = os.path.expanduser("~/Public/Projects/OpenGLViewer/model/renders")
path_output = os.path.expanduser("~/Public/Projects/OpenGLViewer/model/renders")

for model_name in sorted(os.listdir(path_renders)):
	path_render = os.path.join(path_renders, model_name)
	if not os.path.isdir(path_render):
		continue
	path_model = os.path.join(path_3dfront, model_name)
	path_glout = os.path.join(path_output, model_name)
	path_camera = os.path.join(path_render, "camera_pos.txt")

	if not os.path.exists(path_model):
		print("Front 3d model {} not exist!".format(path_model))
		assert False
	if not os.path.exists(path_glout):
		os.mkdir(path_glout)

	#for i in range(20):
	cmd = "{} {} {} {} {}".format(path_renderer, path_model, path_camera, path_glout, 0)
	print(cmd)
	code = os.system(cmd)
	# if code != 0:
	# 	assert False

