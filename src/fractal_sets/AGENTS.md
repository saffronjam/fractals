# src/fractalsets/ — per-fractal compute

`Mandelbrot`, `Julia`, `Buddhabrot`, `Polynomial` each subclass `FractalSet` and provide the actual
escape-time math for every host. The CPU path is **AVX2 SIMD** (4 doubles/iteration) via the engine's
`Saffron/Core/SIMD.h` macros (`SIMD_Double`, `SIMD_Add`, `SIMD_Mul`, `SIMD_GreaterThani`, …).

## The SIMD write-back gotcha (read before touching the CPU loops)

In `Mandelbrot.cpp` / `Julia.cpp`, the per-worker `Compute()` runs the escape loop in `__m256i n`
(four 64-bit lanes), then writes the lanes into `FractalArray`. Extracting lanes from `__m256i` is
**compiler-specific** and must cover gcc/clang or the result is silently dropped:

```cpp
#if defined (_MSC_VER)
    FractalArray[off + 0] = static_cast<int>(n.m256i_i64[3]);   // MSVC union member
    ...
#else // GCC / Clang: no .m256i_i64; extract via aligned store
    alignas(32) long long lanes[4];
    _mm256_store_si256(reinterpret_cast<__m256i*>(lanes), n);
    FractalArray[off + 0] = static_cast<int>(lanes[3]);
    ... // lanes[2], lanes[1], lanes[0]
#endif
```

Lane order is reversed (`lane[3] → x+0`) to match `xPosOffsets = SIMD_Set(0,1,2,3)`. Keep both
branches in sync. A missing gcc branch was the cause of the Linux "CPU host renders black" bug:
neither `__MINGW32__` nor `_MSC_VER` was defined, so iteration counts were never stored.

## Notes

- `Julia::JuliaWorker::Compute()` shutdown path does `WorkerComplete++` (increments the *pointer*, not
  the counter) — a latent bug, but only on the `!Alive` teardown branch.
- `Buddhabrot`/`Polynomial` don't use the `.m256i_i64` pattern (no MSVC-only lane access).
