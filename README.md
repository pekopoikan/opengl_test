```
shaders are little programs that rest on the GPU.
These programs are run for each specific section of the graphics pipeline, they are programs
transforming input into output.
```

```
Vector Types
- vecn: the default vector of n floats
- bvecn: a vector n booleans
- ivecn: a vector of n integers
- uvecn: a vector of n unsigned integers
- dvecn: a vector of n double components
```

```
Uniforms
Uniforms are another way to pass date from our application on the CPU tot he shaders on the GPU.
`glGetUniformformLocation`
`glUniform4f`
```