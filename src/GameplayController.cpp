#include "GameplayController.hpp"

#include "RunRecorder.hpp"
#include "UI/ScreenFlash.hpp"

#include <filesystem>

using namespace geode::prelude;

namespace fpcounter {

    GameplayController& GameplayController::get() {
        static GameplayController instance;
        return instance;
    }

    void GameplayController::onLevelInit(PlayLayer* pl) {
        if (m_overlay) {
            m_overlay->removeFromParentAndCleanup(true);
            m_overlay = nullptr;
        }

        bool counterEnabled = true;
        if (auto mod = Mod::get()) {
            counterEnabled = mod->getSettingValue<bool>("counter-enabled");
        }

        if (counterEnabled && pl) {
            m_overlay = CounterOverlay::create();
            if (m_overlay) {
                pl->addChild(m_overlay, 1000);
            }
        }

        onReset();
    }

    void GameplayController::onReset() {
        m_count = 0;
        if (m_overlay) {
            m_overlay->setCount(m_count);
        }
        RunRecorder::get().beginAttempt();
    }

    void GameplayController::onComplete(PlayLayer* pl) {
        if (!pl || !pl->m_level) return;
        int levelId = static_cast<int>(pl->m_level->m_levelID);
        std::string levelName = pl->m_level->m_levelName;
        RunRecorder::get().commit(levelId, levelName);
    }

    void GameplayController::onQuit() {
        if (m_overlay) {
            m_overlay->removeFromParentAndCleanup(true);
            m_overlay = nullptr;
        }
    }

    void GameplayController::onFramePerfect(PlayLayer* pl, FPResult const& result) {
        RunRecorder::get().addEvent(result);

        m_count++;
        if (m_overlay) {
            m_overlay->setCount(m_count);
        }

        auto mod = Mod::get();
        if (!mod) return;

        if (mod->getSettingValue<bool>("flash-enabled") && pl) {
            ScreenFlash::trigger(pl);
        }

        if (mod->getSettingValue<bool>("sound-enabled")) {
            auto path = mod->getSettingValue<std::filesystem::path>("sound-file");
            std::error_code ec;
            if (!path.empty() && std::filesystem::exists(path, ec)) {
                FMODAudioEngine::sharedEngine()->playEffect(path.string());
            }
        }
    }

} // namespace fpcounter
