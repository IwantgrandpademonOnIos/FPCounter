#include "FPDetector.hpp"
#include "GameplayController.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace fpcounter;

namespace {
    void handleInput(PlayerObject* player) {
        auto* pl = PlayLayer::get();
        if (!pl) return;
        if (player != pl->m_player1 && player != pl->m_player2) return;

        auto result = FPDetector::evaluate(pl, player);
        if (result) {
            GameplayController::get().onFramePerfect(pl, *result);
        }
    }
}

class $modify(FPCPlayerObject, PlayerObject){
    bool pushButton(PlayerButton button){bool result = PlayerObject::pushButton(button);
handleInput(this);
return result;
}

bool releaseButton(PlayerButton button) {
    bool result = PlayerObject::releaseButton(button);
    handleInput(this);
    return result;
}
}
;
