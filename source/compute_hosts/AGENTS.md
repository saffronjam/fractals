# source/compute_hosts/ — compute backends

Each `FractalSet` owns one host per `HostType` (`source/host.h`): `Cpu`, `GpuComputeShader`,
`GpuPixelShader`. The active host's `ComputeImage()` + `RenderImage()`/`OnRender()` produce the
viewport image. All derive from `Host` (`source/Host.{h,cpp}`).

| File | Host | How it computes |
|------|------|-----------------|
| `CpuHost.{h,cpp}` | `Cpu` | Manually-spawned worker threads (per-set `Worker` struct with `std::condition_variable`), each computing a vertical slice with **AVX2 SIMD** (see `../fractalsets/AGENTS.md`). Writes iteration counts into a raw `int* _fractalArray`, then `RenderImage()` maps them through the palette into an `sf::VertexArray` of points. |
| `ComputeShaderHost.{h,cpp}` | `GpuComputeShader` | OpenGL compute shader (`assets/shaders/*.comp`, via Glad). |
| `GpuHost.{h,cpp}` / `PixelShaderHost.{h,cpp}` | `GpuPixelShader` | Fragment-shader rendering (`assets/shaders/*.frag`). |

## Gotchas

- **CPU host buffer**: `_fractalArray = new int[SimWidth()*SimHeight()]` (no explicit 32-byte align;
  the SIMD path writes lane-by-lane with scalar stores, so alignment is not required for correctness).
  Workers step `x` by 4; if a worker's section width isn't 4-aligned it can write past the row →
  heap corruption (`malloc(): invalid size`). This is the open CPU-host crash.
- `CpuHost::Resize` reallocates `_fractalArray` without freeing the old one (leak; `delete[]` is
  commented out) and re-points every worker at the new buffer.
- The CPU↔GPU host switch lives in `FractalSet`; CPU is the default. GPU paths are independent of the
  CPU SIMD code, so a CPU-only bug leaves GPU rendering correct.
