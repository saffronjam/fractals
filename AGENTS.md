# fractals — agent guide

Real-time Mandelbrot / Julia / Buddhabrot / Polynomial fractal visualizer. C++20, built on the
in-repo **saffron-engine-2d** (SFML 2.x windowing/graphics, ImGui UI, OpenGL compute shaders, Glad,
Box2D, spdlog). Each fractal can be computed several ways ("hosts"): CPU (AVX2 SIMD), GPU compute
shader, or GPU pixel/fragment shader.

## Layout

| Path | What |
|------|------|
| `src/` | The fractals application (layers, fractal sets, compute hosts, palette). See `src/AGENTS.md`. |
| `deps/saffron-engine-2d/` | Engine submodule. See its `AGENTS.md`. |
| `assets/shaders/` | GLSL: `*.comp` (GPU compute), `*.frag`/`*.vert` (GPU pixel). `#version 430`, use `double`/`dvec`. |
| `assets/{Fonts,Textures,Pals}/` | Runtime assets; `pals/` are palette strips sampled by the renderer. |
| `premake5.lua` | Client build script. `deps/saffron-engine-2d/premake5.lua` is the engine module. |

Build configs: `Debug`, `Release`, `Dist` (all optimize except Debug; Dist outputs to `build/dist/<system>/`).

## Build & run

The build is **premake5 → native toolchain**. The same tree builds on Windows (MSVC) and Linux (gcc);
all platform differences are behind `filter "system:windows"` / `filter "system:linux"` in the premake
scripts. Regenerate project files after editing any `premake5.lua`.

### Windows
`scripts/GenerateProject.bat` → open `saffron.sln` → build (uses the vendored Windows prebuilts under
`deps/saffron-engine-2d/deps/*/lib`).

### Linux — this machine is Fedora Silverblue (immutable; no host `dnf`/`g++`/`cmake`)
Build and run inside the **`fractals-build` toolbox** container (gcc, make, SFML 2.6, Box2D, zenity, gdb;
`premake5` at `~/tools/premake5`):
```
toolbox run -c fractals-build bash -lc '
  cd /var/home/saffronjam/repos/fractals
  ~/tools/premake5 gmake2            # master Makefile at repo root, project .make under build/
  make config=release fractals'      # -> build/bin/Release-linux-x86_64/fractals/fractals
```
Run (SFML uses X11/XWayland, so a display must be passed):
```
toolbox run -c fractals-build bash -lc '
  cd build/bin/Release-linux-x86_64/fractals
  export DISPLAY=:0 WAYLAND_DISPLAY=wayland-0 XDG_RUNTIME_DIR=/run/user/$(id -u)
  exec timeout <secs> ./fractals'    # detached "nohup &" does NOT survive `toolbox run`; use exec
```
Linux deps not in the vendored Windows tree are built from src into `lib/linux/`: Box2D 2.3.1
(`libBox2D.a`) and Glad (`libGlad.a`); SFML is the system package. gdb in the toolbox needs
`SHELL=/bin/bash` + `set startup-with-shell off`.

## Architecture (flow)

`entry_point.h main()` → `ProjectApp` (`src/ProjectApp.cpp`) pushes `BaseLayer`/`ProjectLayer`. Each
frame the active layer drives `FractalManager`, which owns one `FractalSet` per type and routes
`OnUpdate`/`OnRender` to the **active set's active host**. The engine's `ViewportPane` shows the set's
render target; the right-hand ImGui panel binds the manager's parameters (set, host, palette, zoom,
iterations). Camera/zoom use `double` precision for deep zoom.

## Cross-platform gotchas (all already handled — keep them working)

- **Case sensitivity**: Linux is case-sensitive; includes must match filenames exactly.
- **SIMD result write-back** is per-compiler — see `src/fractalsets/AGENTS.md`. The MSVC `.m256i_i64`
  union member does not exist on gcc/clang.
- **Don't use ES `precision`/`highp` qualifiers** as inline casts in shaders — desktop GLSL rejects them.
- **Asset copy**: `utils.lua` `CopyCmd` merges directory *contents* on POSIX (`cp dir dir/` nests).
- **PCH** (`saffron_pch.h`) is enabled only on Windows; every `.cpp` `#include`s it explicitly anyway.

## Known issues

- CPU host can abort with `malloc(): invalid size` — heap corruption from the worker writing
  `FractalArray` out of bounds (section widths not 4-aligned). Pre-existing; surfaced by glibc. The
  fractal renders before it trips.

Nested guides: `src/AGENTS.md`, `src/compute_hosts/AGENTS.md`, `src/fractalsets/AGENTS.md`,
`deps/saffron-engine-2d/AGENTS.md`.
