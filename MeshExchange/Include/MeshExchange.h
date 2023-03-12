#pragma once

#include <string>
#include <Group.h>
#include <IntrusivePtr.h>

class MeshExchange {
public:
    static IntrusivePtr<Group> LoadObj(std::string path);
};