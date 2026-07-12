#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/TextInput.hpp>

namespace fpcounter {

    class ScanPopup : public geode::Popup {
    public:
        static ScanPopup* create(int prefillLevelId);

    protected:
        bool init(int prefillLevelId);
        void runScan(int levelId);
        void onScan(cocos2d::CCObject*);

    private:
        geode::TextInput* m_idInput = nullptr;
        geode::ScrollLayer* m_resultsScroll = nullptr;
        cocos2d::CCLabelBMFont* m_statusLabel = nullptr;
    };

} // namespace fpcounter
