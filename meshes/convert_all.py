import os

current_dir = os.getcwd() + "/meshes"
for file in os.listdir(current_dir):
	if file.endswith(".obj") or file.endswith(".dae"):
		input_file = current_dir + "/" + file
		output_file = current_dir + "/" + file[:-3] + "bin"
		os.system("./model_convert " + input_file + " " + output_file)

