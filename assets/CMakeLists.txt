cmake_minimum_required (VERSION 3.17)

project(assets NONE)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/demo/assets)

# Config .ini files
configure_file(config/config.ini ${OUTPUT_DIR}/config/config.ini COPYONLY)

# Models
configure_file(models/model.obj ${OUTPUT_DIR}/models/model.obj COPYONLY)

# Textures
configure_file(textures/model.png ${OUTPUT_DIR}/textures/model.png COPYONLY)
configure_file(textures/model2.png ${OUTPUT_DIR}/textures/model2.png COPYONLY)
configure_file(textures/model3.png ${OUTPUT_DIR}/textures/model3.png COPYONLY)
