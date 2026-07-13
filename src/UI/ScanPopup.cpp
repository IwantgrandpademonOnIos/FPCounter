#include "ScanPopup.hpp"

#include "../RunRecorder.hpp"

using namespace geode::prelude;

namespace fpcounter {

    namespace {
        ccColor3B tierColor(int requiredFps) {
            if (requiredFps >= 240) return {255, 90, 90}; // tightest inputs, red
            if (requiredFps >= 120) return {255, 180, 60}; // orange
            return {255, 255, 255}; // 60fps baseline, plain white
        }
    }

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
        if (!Popup::init(400.f, 290.f)) return false;

        this->setTitle("Frame Perfect Scanner");

        auto inputPanel = CCScale9Sprite::create("GJ_square02.png");
        inputPanel->setContentSize({360.f, 44.f});
        inputPanel->setOpacity(90);
        inputPanel->setPosition({m_size.width / 2.f, m_size.height - 52.f});
        m_mainLayer->addChild(inputPanel);

        m_idInput = TextInput::create(160.f, "Level ID", "chatFont.fnt");
        m_idInput->setCommonFilter(CommonFilter::Uint);
        m_idInput->setPosition({m_size.width / 2.f - 65.f, m_size.height - 52.f});
        m_mainLayer->addChild(m_idInput);

        auto scanSpr = ButtonSprite::create("Scan", "bigFont.fnt", "GJ_button_01.png", 0.9f);
        scanSpr->setScale(0.75f);
        auto scanBtn = CCMenuItemSpriteExtra::create(scanSpr, this, menu_selector(ScanPopup::onScan));

        auto menu = CCMenu::create();
        menu->addChild(scanBtn);
        menu->setPosition({m_size.width / 2.f + 95.f, m_size.height - 52.f});
        m_mainLayer->addChild(menu);

        m_statusLabel = CCLabelBMFont::create("Enter a level ID and tap Scan.", "chatFont.fnt");
        m_statusLabel->setScale(0.55f);
        m_statusLabel->setPosition({m_size.width / 2.f, m_size.height - 88.f});
        m_mainLayer->addChild(m_statusLabel);

        auto headerRow =
            CCLabelBMFont::create("PROGRESS      FPS NEEDED      MARGIN", "chatFont.fnt");
        headerRow->setScale(0.4f);
        headerRow->setOpacity(140);
        headerRow->setAnchorPoint({0.f, 0.5f});
        headerRow->setPosition({24.f, m_size.height - 112.f});
        m_mainLayer->addChild(headerRow);

        auto resultsPanel = CCScale9Sprite::create("GJ_square02.png");
        resultsPanel->setContentSize({360.f, 150.f});
        resultsPanel->setOpacity(60);
        resultsPanel->setPosition({m_size.width / 2.f, 90.f});
        m_mainLayer->addChild(resultsPanel);

        m_resultsScroll = ScrollLayer::create({350.f, 140.f});
        m_resultsScroll->setPosition({m_size.width / 2.f - 175.f, 20.f});
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
            m_resultsScroll->m_contentLayer->setContentSize({350.f, 140.f});
            return;
        }

        m_statusLabel->setString(fmt::format(
                                     "{} frame perfects found in your last clear of \"{}\"",
                                     run->events.size(),
                                     run->levelName
        )
                                     .c_str());

        float y = -6.f;
        bool alternate = false;
        for (auto const& ev : run->events) {
            if (alternate) {
                auto stripe = CCLayerColor::create(ccc4(255, 255, 255, 12));
                stripe->setContentSize({350.f, 18.f});
                stripe->setPosition({0.f, y - 15.f});
                stripe->ignoreAnchorPointForPosition(false);
                stripe->setAnchorPoint({0.f, 0.f});
                m_resultsScroll->m_contentLayer->addChild(stripe);
            }
            alternate = !alternate;

            auto row = CCLabelBMFont::create(
                fmt::format(
                    "{:>5.1f}%          {:>3} fps          {:>5.1f}px", ev.percent, ev.requiredFps, ev.clearance
                )
                    .c_str(),
                "chatFont.fnt"
            );
            row->setScale(0.45f);
            row->setAnchorPoint({0.f, 1.f});
            row->setPosition({8.f, y});
            row->setColor(tierColor(ev.requiredFps));
            m_resultsScroll->m_contentLayer->addChild(row);
            y -= 18.f;
        }

        m_resultsScroll->m_contentLayer->setContentSize({350.f, std::max(140.f, -y)});
    }

} // namespace fpcounter
