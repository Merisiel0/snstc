SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/src/assets/shaders"

# skybox
glslc $SCRIPT_DIR/uncompiled/skybox.vert --target-env=vulkan1.3 -o $SCRIPT_DIR/skybox_vert.spv
glslc $SCRIPT_DIR/uncompiled/skybox.frag --target-env=vulkan1.3 -o $SCRIPT_DIR/skybox_frag.spv

# light algorithms
glslc $SCRIPT_DIR/uncompiled/unlit.frag --target-env=vulkan1.3 -o $SCRIPT_DIR/frag0000.spv

# mesh layouts
glslc $SCRIPT_DIR/uncompiled/staticmesh.vert --target-env=vulkan1.3 -o $SCRIPT_DIR/vert0000.spv

# glslc $SCRIPT_DIR/uncompiled/PBR_material.vert --target-env=vulkan1.3 -o $SCRIPT_DIR/PBR_material_vert.spv
# glslc $SCRIPT_DIR/uncompiled/PBR_material.frag --target-env=vulkan1.3 -o $SCRIPT_DIR/PBR_material_frag.spv
# glslc $SCRIPT_DIR/uncompiled/color_material.vert --target-env=vulkan1.3 -o $SCRIPT_DIR/color_material_vert.spv
# glslc $SCRIPT_DIR/uncompiled/color_material.frag --target-env=vulkan1.3 -o $SCRIPT_DIR/color_material_frag.spv