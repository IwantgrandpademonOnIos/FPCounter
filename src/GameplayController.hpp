#pragma once

#include "FPDetector.hpp"
#include "UI/CounterOverlay.hpp"

#include <Geode/Geode.hpp>

namespace fpcounter {

    class GameplayController {
    public:
        static GameplayController& get();

        void onLevelInit(PlayLayer* pl);
        void onReset();
        void onComplete(PlayLayer* pl);
        void onQuit();

        void onFramePerfect(PlayLayer* pl, FPResult const& result);

    private:
        CounterOverlay* m_overlay = nullptr;
        int m_count = 0;
    };

} // namespace fpcounter
