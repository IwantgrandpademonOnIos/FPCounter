#include "UI/ScanPopup.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;
using namespace fpcounter;

class $modify(FPCMenuLayer, MenuLayer){bool init(){if (!MenuLayer::init()) return false;

auto winSize = CCDirector::sharedDirector()->getWinSize();

auto spr = ButtonSprite::create("FP", "bigFont.fnt", "GJ_button_04.png", 0.9f);
spr->setScale(0.6f);
auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FPCMenuLayer::onFPScan));

auto menu = CCMenu::create();
menu->addChild(btn);
menu->setPosition({28.f, winSize.height - 28.f});
this->addChild(menu, 100);

return true;
}

void onFPScan(CCObject*) {
    if (auto popup = ScanPopup::create(0)) {
        popup->show();
    }
}
}
;

class $modify(FPCPauseLayer, PauseLayer){void customSetup(){PauseLayer::customSetup();

auto winSize = CCDirector::sharedDirector()->getWinSize();

auto spr = ButtonSprite::create("FP Scan", "bigFont.fnt", "GJ_button_04.png", 0.7f);
spr->setScale(0.5f);
auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FPCPauseLayer::onFPScan));

auto menu = CCMenu::create();
menu->addChild(btn);
menu->setPosition({winSize.width - 40.f, 40.f});
this->addChild(menu, 100);
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
}
;
