#include "GameplayController.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace fpcounter;

class $modify(FPCPlayLayer, PlayLayer){
    bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects){
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
GameplayController::get().onLevelInit(this);
return true;
}

void resetLevel() {
    PlayLayer::resetLevel();
    GameplayController::get().onReset();
}

void levelComplete() {
    GameplayController::get().onComplete(this);
    PlayLayer::levelComplete();
}

void onQuit() {
    GameplayController::get().onQuit();
    PlayLayer::onQuit();
}
}
;
