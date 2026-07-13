#pragma once

namespace fpcounter {

    // GD does not always simulate physics at a fixed 240 ticks/sec - the actual
    // tick length matches whatever the current attempt is running at (which
    // macros and TPS-changing mods can set to anything: 60, 120, 240...).
    // GJBaseGameLayerHooks.cpp caches the real per-tick dt here every time
    // GJBaseGameLayer::getModifiedDelta runs, so FPDetector always measures
    // "one tick of movement" against the rate actually being simulated instead
    // of assuming a fixed rate.
    class TickContext {
    public:
        static double currentDt();
        static void setCurrentDt(double dt);

    private:
        static inline double s_currentDt = 1.0 / 240.0;
    };

} // namespace fpcounter
