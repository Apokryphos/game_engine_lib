#!/bin/bash

# Compiles shaders

VULKAN_SDK_BIN_PATH=extlibs/vulkan_1.2.141.2/x86_64/bin
INPUT_PATH=assets/shaders/vk
OUTPUT_PATH=build/demo/assets/shaders/vk

mkdir -p $OUTPUT_PATH
$VULKAN_SDK_BIN_PATH/glslc $INPUT_PATH/model.vert -o $OUTPUT_PATH/vert.spv
$VULKAN_SDK_BIN_PATH/glslc $INPUT_PATH/model.frag -o $OUTPUT_PATH/frag.spv
