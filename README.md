```
-GL_REPEAT: The default behaviour of textures. Repeats the texture image.
-GL_MIRRORED_REPEAT: mirrors the image with each repeat
-GL_CLAMP_TO_EDGE: clamps the coordinates between 0 and 1.
-GL_CLAMP_TO_BORDER: Coordinates outside the range are now given a user-specified border color
```

```
Coordinate systems
- Local Space (or Object Space)
- World Space
- View space
- Clip space
- Screen space
```

```
![Coodinate Transformation process](https://learnopengl.com/img/getting-started/coordinate_systems.png)
```

```
![Pic](https://learnopengl.com/img/model_loading/assimp_structure.png)
```

```
By setting aiProcess_Triangulate we tell Assimp that if the model does not (entirely) consist of triangles, it should transform all the model's primitive shapes to triangles first.
```

```
The aiProcess_FlipUVs flips the texture coordinates on the y-axis where necessary during processing (you may remember from the Textures chapter that most images in OpenGL were reversed around the y-axis; this little postprocessing option fixes that for us)
```

```
-aiProcess_GenNormals: creates normal vectors for each vertex if the model doesn't contain normal vectors.
-aiProcess_SplitLargeMeshes: splits large meshes into smaller sub-meshes which is useful if your rendering has a maximum number of vertices allowed and can only process smaller meshes.
-aiProcess_OptimizeMeshes: does the reverse by trying to join several meshes into one larger mesh, reducing drawing calls for optimization.
```