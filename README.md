# Spherical-Map-Viewer
This project implements an **environment mapping technique** using 360-degree photos captured with a 360 camera.  
By mapping these panoramic images onto a sphere, the viewer can experience a realistic virtual environment.  

</br>

## 📸 Preview

</br>

## ✨ Features

-  Spherical Image Display 
    -  Enter a group name (e.g., `class1`, `class2`, `ny`) to load two images:  
        - `groupname_spheremap.tga`  
        - `groupname_diffusemap.tga`  
    - The `spheremap.tga` image is mapped onto a sphere to display the surrounding environment.

- 🌈 Fresnel Shading
  - Fresnel effect changes reflectivity based on the angle between the viewer and the surface normal.  
  - Implemented in the fragment shader:  

  ```glsl
  float F = 0.0;
  float ratio = F + (1.0 - F) * pow((1.0 + dot(wV, wN)), FresnelPower);
  if (FresnelPower > 10) ratio = 0;
  fColor = mix(phong_color, reflect_color, ratio);
  ```

- 🌍 Diffuse Map Effect 
  - Uses blurred 360-degree images (diffusemap.tga) as a diffuse light map for Image Based Lighting (IBL).
  - The diffuse color is determined by sampling the diffuse map in the direction of the surface normal.
  - Toggle diffuse map with the 3 key.

### 🎮 Controls 

| Input                | Action                                                                 |
|----------------------|-------------------------------------------------------------------------|
| **Left Mouse Drag**  | Rotate the view (updates camera orientation: horizontal & vertical look) |
| **Right Mouse Drag** | Zoom in/out (changes camera distance from the sphere)                   |
| **q**                | Toggle between **sphere** and **rabbit** model                         |
| **1**                | Decrease Fresnel power                                                 |
| **2**                | Increase Fresnel power                                                 |
| **3**                | Toggle diffuse light map                                               |

- The implementation adjusts the **camera’s position and direction** rather than the environment.  
- Rotations are handled in Cartesian coordinates using two angles:  
  - **theta** (longitude)  
  - **phi** (latitude)  



</br>

## 🛠 Implementation Details
### 1. Spherical Mapping

- The spheremap.tga image is loaded and mapped onto a sphere.
- The sphere is scaled so the camera remains inside it.
- Front-face culling is enabled so the front side is not rendered, preventing the environment from being blocked during zoom out.

### 2. Viewing Controls

- Camera position is converted to Cartesian coordinates based on a sphere with adjustable radius.
- Equations:
```
x = r * cos(theta) * cos(phi)
y = r * sin(phi)
z = r * sin(theta) * cos(phi)
```
- Left Mouse → Adjusts theta/phi (rotation)
- Right Mouse → Adjusts radius (zoom)
- Vertical rotation (phi) is clamped to [-π/2, π/2] to prevent flipping.

### 3. Fresnel Shading
- Reflection vector is computed and used to sample environment colors.
  
```
R = normalize(2 * dot(wV, wN) * wN - wV)
```
- **wV**: View direction (camera → surface)
- **wN**: Surface normal
</br>
  
- Texture coordinates for reflection mapping:
```
u = (atan2(Nz, Nx) + PI) / (2 * PI)
v = sin^{-1}(Ny) / PI + 0.5
```
- Sampled color is used as the diffuse component in lighting.

</br>

## 🚀 TrolbleShooting
- **Facing Direction Inside Sphere**
   When zooming out, the camera could see the sphere’s front face, blocking the intended environment. Solved by enabling front-face culling in OpenGL.
  
- **Incorrect Zoom Implementation**
   Initially, zooming was implemented by scaling the sphere and objects, but this does not move the camera itself. Changed to adjust camera’s radius relative to the scene’s origin for correct user experience.
- **Incorrect Reflection Mapping**
   Originally, reflected environment colors were fetched based on pixel position, not the true reflection direction. Fixed by computing the reflection vector for accurate environment-based reflection.

- **Reflection Vector Calculation**
   Used ePos directly, which produced a vector from the surface toward the camera. Instead, needed the direction from the camera to the surface (reverse of ePos).

- **Diffuse Map Usage**
   Early attempts multiplied the diffuse map color by the diffuse lighting value directly, resulting in dark rendering. Corrected by sampling colors from the map in the normal direction and using them as diffuse lighting values.

- Normal-Based Diffuse Mapping: Initially used reflection vectors for diffuse mapping, which produced incorrect lighting. Corrected by using the surface normal.



