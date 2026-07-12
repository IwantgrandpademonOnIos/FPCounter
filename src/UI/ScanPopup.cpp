#include "ScanPopup.hpp"

#include "../RunRecorder.hpp"

using namespace geode::prelude;

namespace fpcounter {

    ScanPopup* ScanPopup::create(int prefillLevelId) {
        auto ret = new ScanPopup();
        if (ret && ret->init(prefillLevelId)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool ScanPopup::init(int prefillLevelId) {
        if (!Popup::init(360.f, 260.f)) return false;

        this->setTitle("Frame Perfect Scanner");

        m_idInput = TextInput::create(140.f, "Level ID", "chatFont.fnt");
        m_idInput->setCommonFilter(CommonFilter::Uint);
        m_idInput->setPosition({m_size.width / 2.f - 40.f, m_size.height - 55.f});
        m_mainLayer->addChild(m_idInput);

        auto scanSpr = ButtonSprite::create("Scan", "bigFont.fnt", "GJ_button_01.png", 0.8f);
        scanSpr->setScale(0.7f);
        auto scanBtn = CCMenuItemSpriteExtra::create(scanSpr, this, menu_selector(ScanPopup::onScan));

        auto menu = CCMenu::create();
        menu->addChild(scanBtn);
        menu->setPosition({m_size.width / 2.f + 65.f, m_size.height - 55.f});
        m_mainLayer->addChild(menu);

        m_statusLabel = CCLabelBMFont::create("Enter a level ID and tap Scan.", "chatFont.fnt");
        m_statusLabel->setScale(0.5f);
        m_statusLabel->setPosition({m_size.width / 2.f, m_size.height - 90.f});
        m_mainLayer->addChild(m_statusLabel);

        m_resultsScroll = ScrollLayer::create({m_size.width - 40.f, 130.f});
        m_resultsScroll->setPosition({20.f, 30.f});
        m_mainLayer->addChild(m_resultsScroll);

        if (prefillLevelId > 0) {
            m_idInput->setString(std::to_string(prefillLevelId));
            runScan(prefillLevelId);
        }

        return true;
    }

    void ScanPopup::onScan(CCObject*) {
        std::string idStr = m_idInput->getString();
        if (idStr.empty()) {
            m_statusLabel->setString("Enter a level ID first.");
            return;
        }
        int levelId = 0;
        try {
            levelId = std::stoi(idStr);
        }
        catch (...) {
            m_statusLabel->setString("That doesn't look like a valid level ID.");
            return;
        }
        runScan(levelId);
    }

    void ScanPopup::runScan(int levelId) {
        m_resultsScroll->m_contentLayer->removeAllChildren();

        auto run = RunRecorder::load(levelId);
        if (!run) {
            m_statusLabel->setString(
                fmt::format("No recorded clear for level {}. Play through it once with FP Counter enabled first.", levelId)
                    .c_str()
            );
            return;
        }

        m_statusLabel->setString(fmt::format(
                                     "{} frame perfects found in your last clear of \"{}\"",
                                     run->events.size(),
                                     run->levelName
        )
                                     .c_str());

        float y = 0.f;
        for (auto const& ev : run->events) {
            auto row = CCLabelBMFont::create(
                fmt::format(
                    "{:.1f}%  -  needs {} fps  -  {:.1f}px margin", ev.percent, ev.requiredFps, ev.clearance
                )
                    .c_str(),
                "chatFont.fnt"
            );
            row->setScale(0.45f);
            row->setAnchorPoint({0.f, 1.f});
            row->setPosition({4.f, y});
            m_resultsScroll->m_contentLayer->addChild(row);
            y -= 16.f;
        }

        m_resultsScroll->m_contentLayer->setContentSize({m_size.width - 40.f, std::max(130.f, -y)});
    }

} // namespace fpcounter
