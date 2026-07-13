#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "TickContext.hpp"

using namespace geode::prelude;
using namespace fpcounter;

class $modify(FPCGJBaseGameLayer, GJBaseGameLayer) {
    double getModifiedDelta(float dt) {
        double result = GJBaseGameLayer::getModifiedDelta(dt);
        TickContext::setCurrentDt(result);
        return result;
    }
};
