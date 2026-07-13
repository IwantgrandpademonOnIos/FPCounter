#include "TickContext.hpp"

namespace fpcounter {

    double TickContext::currentDt() {
        return s_currentDt;
    }

    void TickContext::setCurrentDt(double dt) {
        if (dt > 0.0) {
            s_currentDt = dt;
        }
    }

} // namespace fpcounter
