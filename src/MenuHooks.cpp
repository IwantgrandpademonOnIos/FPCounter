#include "UI/ScanPopup.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;
using namespace fpcounter;

class $modify(FPCMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto spr = CircleButtonSprite::create(
            CCLabelBMFont::create("FP", "bigFont.fnt"),
            CircleBaseColor::Green,
            CircleBaseSize::Small
        );
        spr->setScale(0.7f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FPCMenuLayer::onFPScan));

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition({ 27.f, 27.f });
        menu->setZOrder(200);
        this->addChild(menu, 200);

        return true;
    }

    void onFPScan(CCObject*) {
        if (auto popup = ScanPopup::create(0)) {
            popup->show();
        }
    }
};

class $modify(FPCPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto spr = CircleButtonSprite::create(
            CCLabelBMFont::create("FP", "bigFont.fnt"),
            CircleBaseColor::Green,
            CircleBaseSize::Small
        );
        spr->setScale(0.65f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FPCPauseLayer::onFPScan));

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition({ winSize.width - 30.f, 30.f });
        menu->setZOrder(200);
        this->addChild(menu, 200);
    }

    void onFPScan(CCObject*) {
        int levelId = 0;
        if (auto pl = PlayLayer::get()) {
            if (pl->m_level) {
                levelId = static_cast<int>(pl->m_level->m_levelID);
            }
        }
        if (auto popup = ScanPopup::create(levelId)) {
            popup->show();
        }
    }
};
