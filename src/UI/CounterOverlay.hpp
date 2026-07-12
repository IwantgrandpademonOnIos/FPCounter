#pragma once

#include <Geode/Geode.hpp>

namespace fpcounter {

    class CounterOverlay : public cocos2d::CCNode {
    public:
        static CounterOverlay* create();
        void setCount(int count);

    protected:
        bool init() override;

    private:
        cocos2d::CCLabelBMFont* m_label = nullptr;
        int m_count = 0;
    };

} // namespace fpcounter
