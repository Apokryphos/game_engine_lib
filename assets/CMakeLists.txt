cmake_minimum_required (VERSION 3.17)

project(assets NONE)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/demo/assets)

# Config .ini files
configure_file(config/config.ini ${OUTPUT_DIR}/config/config.ini COPYONLY)

# Models
configure_file(models/model.obj ${OUTPUT_DIR}/models/model.obj COPYONLY)
configure_file(models/model2.glb ${OUTPUT_DIR}/models/model2.glb COPYONLY)
configure_file(models/model2.mtl ${OUTPUT_DIR}/models/model2.mtl COPYONLY)

# Textures
configure_file(textures/billboard.png ${OUTPUT_DIR}/textures/billboard.png COPYONLY)
configure_file(textures/model.png ${OUTPUT_DIR}/textures/model.png COPYONLY)
configure_file(textures/model2.png ${OUTPUT_DIR}/textures/model2.png COPYONLY)
configure_file(textures/model3.png ${OUTPUT_DIR}/textures/model3.png COPYONLY)
configure_file(textures/sprite.png ${OUTPUT_DIR}/textures/sprite.png COPYONLY)
