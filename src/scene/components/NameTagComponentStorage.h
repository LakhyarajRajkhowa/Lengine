#pragma once

#include <unordered_map>
#include "NameTagComponent.h"

class NameTagComponentStorage
{
public:

    void Add(uint32_t entityID, const NameTagComponent& tag)
    {
        components[entityID] = tag;
    }

    bool Has(uint32_t entityID) const
    {
        return components.find(entityID) != components.end();
    }

    NameTagComponent& Get(uint32_t entityID)
    {
        return components.at(entityID);
    }

    const NameTagComponent& Get(uint32_t entityID) const
    {
        return components.at(entityID);
    }

    void Remove(uint32_t entityID)
    {
        components.erase(entityID);
    }

private:

    std::unordered_map<uint32_t, NameTagComponent> components;
};