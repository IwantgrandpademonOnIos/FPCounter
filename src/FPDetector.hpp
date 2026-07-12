#pragma once

#include <Geode/Geode.hpp>
#include <optional>

namespace fpcounter {

    // GD's physics simulation runs at a fixed 240 ticks/sec as of 2.2, independent
    // of render FPS. Everything here is measured against that base rate; a result's
    // requiredFps is a conversion from "ticks of margin at 240 TPS" into "the
    // display FPS at which a human clicking once per rendered frame would find
    // this input frame perfect."
    constexpr double kPhysicsTickRate = 240.0;

    struct FPResult {
        float clearance = 0.f; // world-px gap to the nearest relevant hitbox at the moment of input
        float displacement = 0.f; // world-px the player moves in one 240Hz tick at current speed
        double validTicks240 =
            0.0; // clearance / displacement, i.e. how many 240Hz ticks of margin existed
        int requiredFps = 0; // 240 / round(validTicks240), clamped to [30, 240]
        float percent = 0.f; // level progress at the moment of input, for reporting
    };

    class FPDetector {
    public:
        // Call from a PlayerObject::pushButton/releaseButton hook, right after the
        // real button handler ran. Returns a result if this specific input's
        // clearance was tight enough to be a frame perfect, per the "Loosest FPS
        // Still Counted" setting.
        static std::optional<FPResult> evaluate(GJBaseGameLayer* layer, PlayerObject* player);

    private:
        static float rectClearance(cocos2d::CCRect const& a, cocos2d::CCRect const& b);
    };

} // namespace fpcounter
