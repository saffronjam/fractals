# src/ — the fractals application

The client app layered on saffron-engine-2d. (Engine docs: `../deps/saffron-engine-2d/AGENTS.md`.
build/run: `../AGENTS.md`.)

## Files

| File | Role |
|------|------|
| `ProjectApp.{h,cpp}` | `saffron::App` subclass; `CreateApplication()` is the entry the engine calls. Pushes layers. |
| `layers/BaseLayer`, `layers/ProjectLayer` | Engine `Layer`s; per-frame `OnUpdate`/`OnGuiRender` driving the manager + ImGui panels. |
| `FractalManager.{h,cpp}` | Owns all `FractalSet`s, the camera (double precision), and UI binding. Routes update/render to the **active set**. Marks sets for recompute when the view (sim box) changes. |
| `FractalSet.{h,cpp}` | Base for a fractal type. Owns its **hosts** (one per `HostType`) and the active-host switch. Default host is `HostType::Cpu`. `FractalSetPlace` holds named camera presets. |
| `Host.{h,cpp}` | Base compute-host interface. See `compute_hosts/AGENTS.md`. |
| `compute_hosts/` | CPU and GPU host implementations. |
| `fractalsets/` | Per-fractal compute (Mandelbrot, Julia, Buddhabrot, Polynomial). CPU paths use AVX2. |
| `PaletteManager.{h,cpp}` | Loads palette strips from `assets/pals`, exposes a pixel pointer the renderer samples. |
| `Common.{h,cpp}` | `using Position = sf::Vector2<double>;`, `RealType`, GL helpers. In `namespace saffron`. |

## Conventions / gotchas

- Everything is in `namespace saffron`. `Position` is a type alias — **don't name a member `Position`**
  unqualified next to it (gcc `-Wchanges-meaning` error); qualify as `saffron::Position`.
- The render path: a host computes into a target the engine `ViewportPane` displays. A **black
  viewport means the active host produced no output**, not that the app is broken — check that host.
- View changes trigger `RequestImageComputation()` only when the sim box actually changes
  (`FractalManager::OnUpdate`), and only above a min viewport size (≥200 px).

## Verifying a change visually

Build + run (see `../AGENTS.md`), then screenshot the window from the host with ImageMagick:
`xprop -root _NET_CLIENT_LIST` → find the window whose `WM_CLASS` is `"fractals"` →
`import -window <id> out.png`. Toggle Host (CPU/GPU) and Fractal Set in the right-hand panel to
exercise both compute paths.
