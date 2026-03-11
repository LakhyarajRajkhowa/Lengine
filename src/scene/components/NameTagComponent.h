#pragma once

#include <string>

class NameTagComponent
{
public:

    std::string name;

    NameTagComponent()
        : name("MyEntity")
    {
    }

    NameTagComponent(const std::string& name)
        : name(name)
    {
    }
};