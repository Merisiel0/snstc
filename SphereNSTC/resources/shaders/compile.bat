@echo on
"%~dp0glslc.exe" uncompiled\defaultTriangleVertShader.vert --target-env=vulkan1.3 -o defaultTriangleVertShader.spv
"%~dp0glslc.exe" uncompiled\defaultTriangleFragShader.frag --target-env=vulkan1.3 -o defaultTriangleFragShader.spv

"%~dp0glslc.exe" uncompiled\rayGenShader.rgen --target-env=vulkan1.3 -o rayGenShader.spv
"%~dp0glslc.exe" uncompiled\rayChitShader.rchit --target-env=vulkan1.3 -o rayChitShader.spv
"%~dp0glslc.exe" uncompiled\rayMissShader.rmiss --target-env=vulkan1.3 -o rayMissShader.spv
pause