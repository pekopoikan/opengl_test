```
- It takes 3D coordinates as input
- The graphics pipeline transforms 3D coordinates into 2D coordinataes and transforms 2D coordinates into colored pixels
```

```
A vertex is a collection of data per 3D coordinate
```

```
primitives are hints that tell OpenGL how the data should be rendered
- GL_POINTS
- GL_TRIANGLES
- GL_LINE_STRIP
```

```
VBO(Vertex Buffer Object) stures a large number of verices in the GPU's memory
```

```
VAO(Vertex Array Object) makes it so that any subsequent vertex attribute calls are stored inside the VAO.
```

```
EBO(Element Buffer Object) stores indices that OpenGL uses to decide what vertices to draw
```

```
- GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times
- GL_STATIC_DRAW: the data is set only once and used many times
- GL_DYNAMIC_DRAW: the data is changed a lot and used many times
```

```
Shader program object is the final linked version of multiple shaders combined
```