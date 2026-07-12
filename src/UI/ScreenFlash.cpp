#include "ScreenFlash.hpp"

using namespace geode::prelude;

namespace fpcounter {

    void ScreenFlash::trigger(CCNode* parent) {
        if (!parent) return;

        ccColor3B color = {0, 255, 0};
        float duration = 0.18f;
        if (auto mod = Mod::get()) {
            color = mod->getSettingValue<ccColor3B>("flash-color");
            duration = mod->getSettingValue<double>("flash-duration");
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto flash = CCLayerColor::create(ccc4(color.r, color.g, color.b, 110));
        flash->setContentSize(winSize);
        flash->setPosition({0.f, 0.f});
        flash->setZOrder(999);
        flash->ignoreAnchorPointForPosition(false);
        flash->setAnchorPoint({0.f, 0.f});

        parent->addChild(flash, 999);
        flash->runAction(CCSequence::create(
            CCFadeOut::create(duration),
            CCCallFunc::create(flash, callfunc_selector(CCNode::removeFromParent)),
            nullptr
        ));
    }

} // namespace fpcounter
