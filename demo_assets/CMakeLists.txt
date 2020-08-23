cmake_minimum_required (VERSION 3.17)

project(demo_assets NONE)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/demo/assets)

# Config .ini files
configure_file(config/config.ini ${OUTPUT_DIR}/config/config.ini COPYONLY)

# Models
configure_file(models/model.obj ${OUTPUT_DIR}/models/model.obj COPYONLY)
configure_file(models/model2.glb ${OUTPUT_DIR}/models/model2.glb COPYONLY)
configure_file(models/model2.mtl ${OUTPUT_DIR}/models/model2.mtl COPYONLY)

# Spine
configure_file(spine/spineboy/spineboy.atlas ${OUTPUT_DIR}/spine/spineboy/spineboy.atlas COPYONLY)
configure_file(spine/spineboy/spineboy.png ${OUTPUT_DIR}/spine/spineboy/spineboy.png COPYONLY)
configure_file(spine/spineboy/spineboy.json ${OUTPUT_DIR}/spine/spineboy/spineboy.json COPYONLY)
configure_file(spine/goblins/goblins.atlas ${OUTPUT_DIR}/spine/goblins/goblins.atlas COPYONLY)
configure_file(spine/goblins/goblins.png ${OUTPUT_DIR}/spine/goblins/goblins.png COPYONLY)
configure_file(spine/goblins/goblins.json ${OUTPUT_DIR}/spine/goblins/goblins.json COPYONLY)

# Textures
configure_file(textures/billboard.png ${OUTPUT_DIR}/textures/billboard.png COPYONLY)
configure_file(textures/cp437_20x20.png ${OUTPUT_DIR}/textures/cp437_20x20.png COPYONLY)
configure_file(textures/missing.png ${OUTPUT_DIR}/textures/missing.png COPYONLY)
configure_file(textures/model.png ${OUTPUT_DIR}/textures/model.png COPYONLY)
configure_file(textures/model2.png ${OUTPUT_DIR}/textures/model2.png COPYONLY)
configure_file(textures/model3.png ${OUTPUT_DIR}/textures/model3.png COPYONLY)
configure_file(textures/sprite.png ${OUTPUT_DIR}/textures/sprite.png COPYONLY)

# Glyphs
file(GLOB files "textures/cp437_20x20/*.png")
foreach(file ${files})
    get_filename_component(filename ${file} NAME)
    configure_file(${file} ${OUTPUT_DIR}/textures/cp437_20x20/${filename} COPYONLY)
endforeach()
