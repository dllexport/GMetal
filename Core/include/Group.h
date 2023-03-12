#pragma once

#include <Node.h>
#include <IntrusivePtr.h>
#include <vector>

class Group : public Node
{
public:
	std::vector<IntrusivePtr<Node>> children;
};