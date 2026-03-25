local a, b, c = os.execute([[
glslc -fshader-stage=fragment shaders/fragment.glsl -o build/linux/x86_64/release/shaders/fragment.spv &&
glslc -fshader-stage=vertex shaders/vertex.glsl -o build/linux/x86_64/release/shaders/vertex.spv
]])

assert(a, c)
print('Compiled shaders')
