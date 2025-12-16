#pragma once
#include <string>
#include <vector>
#include <mutex>

class LogBuffer {
public:
    void Add(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex);
        logs.push_back(msg);
    }

    const std::vector<std::string>& GetLogs() const {
        return logs;
    }

private:
    std::vector<std::string> logs;
    mutable std::mutex mutex;
};

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>

inline void DebugLogCooldown(const std::string& input,
    const char* func,
    const char* file,
    int gapMs)
{
    using clock = std::chrono::steady_clock;
    static auto lastTime = clock::now() - std::chrono::milliseconds(gapMs);

    auto now = clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count() < gapMs)
        return;

    lastTime = now;

    std::string fileName = std::filesystem::path(file).filename().string();
    std::cout << "[" << func << "/" << fileName << "] : "
        << input << std::endl;
}

#define DEBUG_LOG_GAP(msg, ms) DebugLogCooldown(msg, __FUNCTION__, __FILE__, ms)

inline void DebugLog(const std::string& input,
    const char* func,
    const char* file)
{
    std::string fileName = std::filesystem::path(file).filename().string();

    std::cout << "[" << func << "/" << fileName << "] : "
        << input << std::endl;
}

#define DEBUG_LOG(msg) DebugLog(msg, __FUNCTION__, __FILE__)
