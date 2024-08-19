@echo on
"%~dp0\glslc.exe" uncompiled\rayGenShader.rgen -o rayGenShader.spv
"%~dp0\glslc.exe" uncompiled\rayChitShader.rchit -o rayChitShader.spv
"%~dp0\glslc.exe" uncompiled\rayMissShader.rmiss -o rayMissShader.spv
pause