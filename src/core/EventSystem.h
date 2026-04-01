#pragma once

#include <external/SDL2/SDL.h>
#include <vector>
#include <functional>

namespace Lengine {

    class EventSystem
    {
    public:

        using EventCallback = std::function<void(const SDL_Event&)>;

        void addListener(EventCallback callback);
        void dispatch(const SDL_Event& event);

    private:

        std::vector<EventCallback> listeners;
    };

}