#pragma once

#include <string>
#include <IntrusivePtr.h>

class Node : public IntrusiveCounter<Node> {
public:
    Node(std::string name) : name(name) {}
    Node() {}
    virtual ~Node() {}
protected:
    std::string name;
};