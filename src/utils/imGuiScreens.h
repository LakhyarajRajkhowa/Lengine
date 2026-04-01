#pragma once
#include <imgui/imgui.h>
#include <atomic>
#include <thread>

extern std::atomic<bool> isLoading;
extern std::atomic<float> loadingProgress;
extern std::thread loadingThread;
extern bool meshReady;


void ShowLoadingPopup();   // don't make it inline
