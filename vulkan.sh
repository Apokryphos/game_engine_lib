#!/bin/bash

# Initializes Vulkan environment variables using copy in extlibs
# This is required to use validation layers for debug builds
export VULKAN_SDK=$PWD/extlibs/vulkan_1.2.141.2/x86_64
export PATH=$VULKAN_SDK/bin:$PATH
export LD_LIBRARY_PATH=$VULKAN_SDK/lib:$LD_LIBRARY_PATH
export VK_LAYER_PATH=$VULKAN_SDK/etc/vulkan/explicit_layer.d

echo Vulkan environment variables exported.
echo To execute in context of current shell: . vulkan.sh
