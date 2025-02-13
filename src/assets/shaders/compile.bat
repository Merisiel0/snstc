@echo on
"%~dp0glslc.exe" uncompiled\PBR_material.vert --target-env=vulkan1.3 -o PBR_material_vert.spv
"%~dp0glslc.exe" uncompiled\PBR_material.frag --target-env=vulkan1.3 -o PBR_material_frag.spv

"%~dp0glslc.exe" uncompiled\color_material.vert --target-env=vulkan1.3 -o color_material_vert.spv
"%~dp0glslc.exe" uncompiled\color_material.frag --target-env=vulkan1.3 -o color_material_frag.spv
pause