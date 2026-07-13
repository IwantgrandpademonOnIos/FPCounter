#pragma once

#include <Geode/Geode.hpp>
#include <optional>

namespace fpcounter {

    struct FPResult {
        float clearance =
            0.f; // world-px gap to the nearest hazard/solid hitbox at the moment of input
        float displacement =
            0.f; // world-px the player moves in one tick, at the tick rate this attempt is actually running at
        double validTicks =
            0.0; // clearance / displacement, i.e. how many ticks of margin existed at the current tick rate
        int requiredFps = 0; // currentTps / round(validTicks), clamped to [30, 240]
        float percent = 0.f; // level progress at the moment of input, for reporting
    };

    class FPDetector {
    public:
        // Call from a PlayerObject::pushButton/releaseButton hook, right after the
        // real button handler ran. Returns a result if this specific input's
        // clearance was tight enough to be a frame perfect, per the "Loosest FPS
        // Still Counted" setting. Uses TickContext for the current tick rate, so
        // this works correctly under macros/TPS bypass mods that don't run at 240.
        static std::optional<FPResult> evaluate(GJBaseGameLayer* layer, PlayerObject* player);
    };

} // namespace fpcounter
