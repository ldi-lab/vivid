# vivid
Lightweight & easy to use 3D visualization library.

## Examples

#### ImGUI 
![ ](assets/docs/imgui.png)

#### Primitives
![ ](assets/docs/primitives.png)

#### Screen gradient
![ ](assets/docs/screen_gradient.png)

#### Frame buffer
![ ](assets/docs/frame_buffer.png)

#### Shadow mapping
![ ](assets/docs/shadow_mapping.png)

#### Basic PBR
![ ](assets/docs/basic_pbr.png)

#### PBR and IBL
![ ](assets/docs/pbr_ibl.png)

#### Point Cloud
![ ](assets/docs/point_cloud.png)

## Shader
Supported names of vertex attributes include:

| Attribute | Description                           |
|-----------|---------------------------------------|
| position  | vertex position                       |
| normal    | vertex normal                         |
| tangent   | vertex tangent                        |
| color     | vertex color                          |
| texCoord0 | primary texture coordinate (UV)       |
| texCoord1 | additional texture coordinate         |
| texCoord2 | additional texture coordinate         |
| texCoord3 | additional texture coordinate         |
| texCoord4 | additional texture coordinate         |
| blendWeight | bone blend weights for skinned meshes |
| blendIndices | bone indices for skinned meshes.      |

* When writing a shader, you must use exactly above names to represent the vertex attributes, 
otherwise the shader can't get the location of each attribute automatically. See the `Shader::GetAttributeLocations()` 
function for details.

## Build
```bash
cd vivid
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_VIVID_SAMPLES=ON
make -j3
```
then you can run the executable demos in `build/samples` directory.


## Acknowledgements
* [ogl](https://github.com/oframe/ogl) : many demos have heavily referenced ogl
* [LearnOpenGL](https://learnopengl.com/)
* [Three.js](https://threejs.org/)
* [imgui](https://github.com/ocornut/imgui)

## Unlicense
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this software, either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this software dedicate any and all copyright interest in the software to the public domain. We make this dedication for the benefit of the public at large and to the detriment of our heirs and successors. We intend this dedication to be an overt act of relinquishment in perpetuity of all present and future rights to this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to https://unlicense.org