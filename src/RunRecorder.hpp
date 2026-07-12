#pragma once

#include "FPDetector.hpp"

#include <Geode/Geode.hpp>
#include <filesystem>
#include <optional>
#include <vector>

namespace fpcounter {

    struct StoredEvent {
        float percent;
        int requiredFps;
        float clearance;
    };

    struct StoredRun {
        int levelId = 0;
        std::string levelName;
        std::vector<StoredEvent> events;
    };

    // Frame perfects are only meaningful in the context of one continuous,
    // successful attempt (dying and retrying resets the buffer). commit() is
    // called on level completion, and overwrites whatever was previously stored
    // for that level ID with this attempt's data - there's no attempt at merging
    // or ranking across multiple clears, the latest clear wins.
    class RunRecorder {
    public:
        static RunRecorder& get();

        void beginAttempt();
        void addEvent(FPResult const& result);

        size_t currentCount() const {
            return m_current.size();
        }

        void commit(int levelId, std::string const& levelName);
        static std::optional<StoredRun> load(int levelId);

    private:
        std::vector<StoredEvent> m_current;
        static std::filesystem::path pathFor(int levelId);
    };

} // namespace fpcounter
