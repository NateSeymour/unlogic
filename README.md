# Unlogic

A math JIT compiler and graph generator.

## Examples

### JIT-Compiling and Running a Function

```c++
unlogic::Scene scene;
unlogic::Compiler compiler({&unlogic::stdlib, &unlogic::runtime});
auto program = *compiler.Compile("given f(x) := x^2; plot f;");

program(&scene);
```

### Calculator

<p align="center">
    <img src="docs/public/SampleRender.png" width="1000">
</p>

Use `unlogic-calculator`, a Qt/Vulkan-based GUI for `unlogic` to perform graph analysis.