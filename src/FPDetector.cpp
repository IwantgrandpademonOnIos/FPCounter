#include "FPDetector.hpp"

#include <algorithm>
#include <cmath>

using namespace geode::prelude;

namespace fpcounter {

    namespace {
        constexpr float kLookaheadPx = 90.f;
        constexpr float kLookbehindPx = 20.f;
        constexpr double kMinSpeed = 40.0; // px/sec; below this, "clearance" is meaningless noise
    }

    float FPDetector::rectClearance(CCRect const& a, CCRect const& b) {
        float aMinX = a.origin.x, aMaxX = a.origin.x + a.size.width;
        float aMinY = a.origin.y, aMaxY = a.origin.y + a.size.height;
        float bMinX = b.origin.x, bMaxX = b.origin.x + b.size.width;
        float bMinY = b.origin.y, bMaxY = b.origin.y + b.size.height;

        float dx = 0.f, dy = 0.f;
        if (aMaxX <= bMinX) dx = bMinX - aMaxX;
        else if (bMaxX <= aMinX) dx = aMinX - bMaxX;

        if (aMaxY <= bMinY) dy = bMinY - aMaxY;
        else if (bMaxY <= aMinY) dy = aMinY - bMaxY;

        if (dx > 0.f && dy > 0.f) return std::sqrt(dx * dx + dy * dy);
        if (dx > 0.f) return dx;
        if (dy > 0.f) return dy;
        return 0.f; // overlapping right now, not a useful clearance reading
    }

    std::optional<FPResult> FPDetector::evaluate(GJBaseGameLayer* layer, PlayerObject* player) {
        if (!layer || !player) return std::nullopt;

        double vx = player->getCurrentXVelocity();
        double vy = player->getYVelocity();
        double speed = std::sqrt(vx * vx + vy * vy);
        if (speed < kMinSpeed) return std::nullopt;

        double displacement = speed / kPhysicsTickRate;
        if (displacement <= 0.0) return std::nullopt;

        CCRect prect = player->getObjectRect();
        CCRect searchRect(
            prect.origin.x - kLookbehindPx,
            prect.origin.y - kLookaheadPx * 0.5f,
            prect.size.width + kLookbehindPx + kLookaheadPx,
            prect.size.height + kLookaheadPx
        );

        auto* blocks = layer->m_collisionBlocks;
        if (!blocks || blocks->count() == 0) return std::nullopt;

        float bestClearance = -1.f;
        for (unsigned int i = 0; i < blocks->count(); i++) {
            auto* obj = static_cast<GameObject*>(blocks->objectAtIndex(i));
            if (!obj) continue;

            CCRect orect = obj->getObjectRect();
            bool overlaps =
                !(orect.origin.x > searchRect.origin.x + searchRect.size.width ||
                  orect.origin.x + orect.size.width < searchRect.origin.x ||
                  orect.origin.y > searchRect.origin.y + searchRect.size.height ||
                  orect.origin.y + orect.size.height < searchRect.origin.y);
            if (!overlaps) continue;

            float clearance = rectClearance(prect, orect);
            if (clearance < 0.f) continue;
            if (bestClearance < 0.f || clearance < bestClearance) {
                bestClearance = clearance;
            }
        }

        if (bestClearance < 0.f) return std::nullopt;

        double validTicks240 = bestClearance / displacement;

        int maxFpsSetting = 60;
        if (auto mod = Mod::get()) {
            maxFpsSetting = static_cast<int>(mod->getSettingValue<int64_t>("max-fps-target"));
        }
        double maxValidTicks = kPhysicsTickRate / static_cast<double>(maxFpsSetting);
        if (validTicks240 > maxValidTicks) return std::nullopt;

        long long roundedTicks = std::max<long long>(1, std::llround(validTicks240));
        int requiredFps =
            static_cast<int>(std::round(kPhysicsTickRate / static_cast<double>(roundedTicks)));
        requiredFps = std::clamp(requiredFps, 30, 240);

        FPResult result;
        result.clearance = bestClearance;
        result.displacement = static_cast<float>(displacement);
        result.validTicks240 = validTicks240;
        result.requiredFps = requiredFps;

        if (auto pl = PlayLayer::get()) {
            result.percent = pl->getCurrentPercent();
        }

        return result;
    }

} // namespace fpcounter
