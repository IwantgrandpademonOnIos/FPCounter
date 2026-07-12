#include "CounterOverlay.hpp"

using namespace geode::prelude;

namespace fpcounter {

    CounterOverlay* CounterOverlay::create() {
        auto node = new CounterOverlay();
        if (node && node->init()) {
            node->autorelease();
            return node;
        }
        CC_SAFE_DELETE(node);
        return nullptr;
    }

    bool CounterOverlay::init() {
        if (!CCNode::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        m_label = CCLabelBMFont::create("FP: 0", "bigFont.fnt");
        m_label->setScale(0.45f);
        m_label->setAnchorPoint({1.f, 1.f});
        m_label->setPosition({winSize.width - 12.f, winSize.height - 12.f});
        m_label->setOpacity(200);
        this->addChild(m_label);

        this->setZOrder(1000);
        this->setContentSize(winSize);
        return true;
    }

    void CounterOverlay::setCount(int count) {
        m_count = count;
        m_label->setString(fmt::format("FP: {}", m_count).c_str());
    }

} // namespace fpcounter
