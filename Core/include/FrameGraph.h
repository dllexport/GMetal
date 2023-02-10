#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <FrameGraphNode.h>

class FrameGraph : public IntrusiveCounter<FrameGraph>
{
public:
	template<class T, class ...Args>
	IntrusivePtr<T> CreateNode(std::string name, Args&& ... args)
	{
		auto p = gmetal::make_intrusive<T>(std::forward<Args>(args)...);
		p->fg = this;
		p->name = name;
		this->nodes.push_back(p);
		this->nodesInMap[p];
		this->nodesOutMap[p];
		return p;
	}

	void Link(IntrusivePtr<FrameGraphNode>&& from, IntrusivePtr<FrameGraphNode>&& to);

	void TopoSort();
	void Compile();

private:
	std::vector<IntrusivePtr<FrameGraphNode>> nodes;
	std::unordered_map<IntrusivePtr<FrameGraphNode>, std::vector<IntrusivePtr<FrameGraphNode>>> nodesInMap;
	std::unordered_map<IntrusivePtr<FrameGraphNode>, std::vector<IntrusivePtr<FrameGraphNode>>> nodesOutMap;

	std::unordered_set<IntrusivePtr<FrameGraphNode>> attachmentsNode;
};