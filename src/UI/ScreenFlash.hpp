#pragma once

#include <Geode/Geode.hpp>

namespace fpcounter {

    class ScreenFlash {
    public:
        // Spawns a full-screen color flash on the given parent that fades out
        // and removes itself. color/duration are read from mod settings if not
        // overridden here.
        static void trigger(cocos2d::CCNode* parent);
    };

} // namespace fpcounter
