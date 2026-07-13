#include "FPDetector.hpp"

#include "TickContext.hpp"

#include <algorithm>
#include <cmath>

using namespace geode::prelude;

namespace fpcounter {

    namespace {
        constexpr float kLookaheadPx = 90.f;
        constexpr float kLookbehindPx = 20.f;
        constexpr double kMinSpeed = 40.0; // px/sec; below this, "clearance" is meaningless noise

        bool rectsOverlap(CCRect const& a, CCRect const& b) {
            return !(
                a.origin.x > b.origin.x + b.size.width || a.origin.x + a.size.width < b.origin.x ||
                a.origin.y > b.origin.y + b.size.height || a.origin.y + a.size.height < b.origin.y
            );
        }

        float rectClearance(CCRect const& a, CCRect const& b) {
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

        // Scans a hazard/solid collision vector for the closest hitbox to the
        // player that falls inside searchRect, updating bestClearance in place.
        void scanForNearest(
            gd::vector<GameObject*> const& objects, int activeCount, CCRect const& prect,
            CCRect const& searchRect, float& bestClearance
        ) {
            int count = std::min<int>(activeCount, static_cast<int>(objects.size()));
            for (int i = 0; i < count; i++) {
                GameObject* obj = objects[i];
                if (!obj) continue;

                CCRect orect = obj->getObjectRect();
                if (!rectsOverlap(orect, searchRect)) continue;

                float clearance = rectClearance(prect, orect);
                if (clearance < 0.f) continue;
                if (bestClearance < 0.f || clearance < bestClearance) {
                    bestClearance = clearance;
                }
            }
        }
    }

    std::optional<FPResult> FPDetector::evaluate(GJBaseGameLayer* layer, PlayerObject* player) {
        if (!layer || !player) return std::nullopt;

        double vx = player->getCurrentXVelocity();
        double vy = player->getYVelocity();
        double speed = std::sqrt(vx * vx + vy * vy);
        if (speed < kMinSpeed) return std::nullopt;

        double dt = TickContext::currentDt();
        if (dt <= 0.0) return std::nullopt;
        double currentTps = 1.0 / dt;

        double displacement = speed * dt;
        if (displacement <= 0.0) return std::nullopt;

        CCRect prect = player->getObjectRect();
        CCRect searchRect(
            prect.origin.x - kLookbehindPx,
            prect.origin.y - kLookaheadPx * 0.5f,
            prect.size.width + kLookbehindPx + kLookaheadPx,
            prect.size.height + kLookaheadPx
        );

        float bestClearance = -1.f;
        scanForNearest(
            layer->m_hazardCollisionObjects, layer->m_hazardCollisionObjectsCount, prect, searchRect, bestClearance
        );
        scanForNearest(
            layer->m_solidCollisionObjects, layer->m_solidCollisionObjectsCount, prect, searchRect, bestClearance
        );

        if (bestClearance < 0.f) return std::nullopt;

        double validTicks = bestClearance / displacement;

        int maxFpsSetting = 60;
        if (auto mod = Mod::get()) {
            maxFpsSetting = static_cast<int>(mod->getSettingValue<int64_t>("max-fps-target"));
        }
        if (maxFpsSetting <= 0) maxFpsSetting = 60;
        double maxValidTicks = currentTps / static_cast<double>(maxFpsSetting);
        if (validTicks > maxValidTicks) return std::nullopt;

        long long roundedTicks = std::max<long long>(1, std::llround(validTicks));
        int requiredFps =
            static_cast<int>(std::round(currentTps / static_cast<double>(roundedTicks)));
        requiredFps = std::clamp(requiredFps, 30, 240);

        FPResult result;
        result.clearance = bestClearance;
        result.displacement = static_cast<float>(displacement);
        result.validTicks = validTicks;
        result.requiredFps = requiredFps;

        if (auto pl = PlayLayer::get()) {
            result.percent = pl->getCurrentPercent();
        }

        return result;
    }

} // namespace fpcounter
