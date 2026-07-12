#include "RunRecorder.hpp"

#include <fstream>
#include <sstream>

using namespace geode::prelude;

namespace fpcounter {

    RunRecorder& RunRecorder::get() {
        static RunRecorder instance;
        return instance;
    }

    void RunRecorder::beginAttempt() {
        m_current.clear();
    }

    void RunRecorder::addEvent(FPResult const& result) {
        m_current.push_back(StoredEvent{result.percent, result.requiredFps, result.clearance});
    }

    std::filesystem::path RunRecorder::pathFor(int levelId) {
        auto dir = Mod::get()->getSaveDir() / "runs";
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        return dir / (std::to_string(levelId) + ".fprun");
    }

    void RunRecorder::commit(int levelId, std::string const& levelName) {
        if (m_current.empty()) return;

        std::ofstream out(pathFor(levelId), std::ios::trunc);
        if (!out.is_open()) {
            log::warn("FPCounter: failed to write run data for level {}", levelId);
            return;
        }

        out << levelId << '\n';
        out << levelName << '\n';
        out << m_current.size() << '\n';
        for (auto const& ev : m_current) {
            out << ev.percent << ',' << ev.requiredFps << ',' << ev.clearance << '\n';
        }
    }

    std::optional<StoredRun> RunRecorder::load(int levelId) {
        auto path = pathFor(levelId);
        std::ifstream in(path);
        if (!in.is_open()) return std::nullopt;

        StoredRun run;
        run.levelId = levelId;

        std::string idLine, nameLine, countLine;
        if (!std::getline(in, idLine)) return std::nullopt;
        if (!std::getline(in, nameLine)) return std::nullopt;
        if (!std::getline(in, countLine)) return std::nullopt;

        run.levelName = nameLine;
        size_t count = 0;
        try {
            count = static_cast<size_t>(std::stoul(countLine));
        }
        catch (...) {
            return std::nullopt;
        }

        run.events.reserve(count);
        std::string line;
        while (std::getline(in, line)) {
            std::stringstream ss(line);
            std::string percentStr, fpsStr, clearanceStr;
            if (!std::getline(ss, percentStr, ',')) continue;
            if (!std::getline(ss, fpsStr, ',')) continue;
            if (!std::getline(ss, clearanceStr, ',')) continue;

            try {
                StoredEvent ev;
                ev.percent = std::stof(percentStr);
                ev.requiredFps = std::stoi(fpsStr);
                ev.clearance = std::stof(clearanceStr);
                run.events.push_back(ev);
            }
            catch (...) {
                continue;
            }
        }

        if (run.events.empty()) return std::nullopt;
        return run;
    }

} // namespace fpcounter
