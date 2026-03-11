#include "EventSystem.h"

namespace Lengine {

    void EventSystem::addListener(EventCallback callback)
    {
        listeners.push_back(callback);
    }

    void EventSystem::dispatch(const SDL_Event& event)
    {
        for (auto& listener : listeners)
        {
            listener(event);
        }
    }

}