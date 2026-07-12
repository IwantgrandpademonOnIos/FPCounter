# FP Counter

[![Build](https://github.com/SSMG4/FPCounter/actions/workflows/build.yml/badge.svg)](https://github.com/SSMG4/FPCounter/actions/workflows/build.yml)
[![Lint](https://github.com/SSMG4/FPCounter/actions/workflows/lint.yml/badge.svg)](https://github.com/SSMG4/FPCounter/actions/workflows/lint.yml)

Geode mod for Geometry Dash 2.2081. Detects genuine frame-perfect inputs live, not "every click" like the joke mods on the index.

## Features

1. **Live detection** - on every push/release, checks the clearance between your hitbox and the nearest collision block against how far you move in one 240Hz physics tick. If the clearance is tighter than the "Loosest FPS Still Counted" setting allows (default 60fps), it counts as a frame perfect.
2. **Screen flash** - tints the screen a color (default green) on each detected frame perfect. Color and fade duration are configurable.
3. **Sound** - plays a sound on each frame perfect. Enable/disable independently of the flash, and pick any audio file from your device (no bundled sound - see Settings).
4. **On-screen counter** - running total for the current attempt, top-right corner.
5. **Scan popup** - button in the main menu and pause menu. Shows every frame perfect from your last recorded clear of a level: percent location, required FPS, and clearance margin. Enter a level ID manually (main menu) or open it mid-level to auto-fill the current level's ID (pause menu).

## Important design note on the Scan feature

There is no general "solve any level and find its frame perfects" algorithm here, and there isn't one anywhere in the GD modding scene either - that's an unsolved problem equivalent to writing a TAS bot capable of beating arbitrary levels blind. What actually happens:

- Every tick, live detection results accumulate into a buffer for the current attempt.
- On `levelComplete()`, that buffer is saved to disk keyed by level ID, overwriting whatever was stored before.
- The Scan popup reads that stored file. If you haven't cleared a level with the mod running, there's nothing to show, and it tells you that directly rather than pretending to solve it.

So "scan the level" really means "show me the analysis from the last time I beat it." Entering an ID you haven't cleared yet just confirms there's no data yet.

## Detection accuracy

The clearance-vs-displacement check is a geometric approximation, not a bit-exact reimplementation of GD's physics. Two constants you'll likely want to tune against real gameplay:

- `kPhysicsTickRate` in `FPDetector.cpp` (240.0, matches GD's fixed post-2.2 tick rate - shouldn't need changing)
- `kLookaheadPx` / `kLookbehindPx` in `FPDetector.cpp` - the window used to find "the nearest relevant object." Too small and it'll miss obstacles at high speed (wave, dash rings); too large and it'll pick up irrelevant background objects.

Rotated hitboxes (diagonal spikes, angled blocks) are checked as axis-aligned rects via `getObjectRect()`, not the oriented box (`getOrientedBox()` exists on `GameObject` if you want to upgrade this later). Expect some false positives/negatives on heavily rotated geometry until that's added.

## Known limitations

- Practice mode checkpoints don't cleanly reset the attempt buffer (only a full `resetLevel()` does), so a checkpoint-heavy practice session's stored "clear" may mix data from before and after a checkpoint placement. Normal mode or checkpoint-free practice runs give the cleanest Scan data.
- Dual mode runs both players through the same detector independently; no special handling for swap-triggers.
- The stored run format is a plain custom text file (`<save dir>/runs/<levelID>.fprun`), not JSON. Geode bundles its own JSON library (`geode-sdk/json`, exposed as `matjson`) but its exact API wasn't verifiable in the sandbox this was written in, so a dependency-free format was used instead. Swap it out if you'd rather have a standard format.

## Building

Requires the [Geode CLI](https://docs.geode-sdk.org/getting-started) and SDK set up (`GEODE_SDK` environment variable pointing at your Geode checkout). This was written and checked against the Geode SDK source and a real published mod's source for API accuracy, and the C++ files are clang-format clean against this repo's `.clang-format` (copied from the Geode SDK's own style), but it has not been compiled or run against the actual game - Geode mods build against Windows/Mac/Android toolchains the sandbox this was built in doesn't have.

```
geode build
```

Output `.geode` file goes to `build/FPCounter.geode` (or wherever your Geode CLI config points); install it via the in-game mod loader or by dropping it in your Geode mods folder.

## CI

- **Build** (`.github/workflows/build.yml`) - builds Windows, macOS, Android32, and Android64 on every push/PR to `main`, combines them into one `.geode`, uploads it as a workflow artifact.
- **Lint** (`.github/workflows/lint.yml`) - checks `src/` against `.clang-format` on every push/PR to `main`.
- **Release** (`.github/workflows/release.yml`) - manual only (`workflow_dispatch`, run it from the Actions tab). Builds every platform, combines them, and publishes a GitHub Release tagged with whatever `version` is currently in `mod.json`. Bump the version before running it - Geode's own docs are explicit that you should never overwrite an existing release, since the package is checksummed.

## Settings

All configurable from the in-game mod settings page: counter on/off, flash on/off + color + fade duration, sound on/off + file picker, and the FPS sensitivity threshold.

## License

MIT, see LICENSE.
