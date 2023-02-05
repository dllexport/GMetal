#include <FrameGraph.h>

#include <queue>
#include <unordered_map>

void FrameGraph::Link(IntrusivePtr<FrameGraphNode>&& from, IntrusivePtr<FrameGraphNode>&& to)
{
	nodesInMap[to].push_back(from);
	nodesOutMap[from].push_back(to);
}

void FrameGraph::TopoSort()
{
	std::unordered_map<IntrusivePtr<FrameGraphNode>, uint32_t> counterMap;
	std::queue<IntrusivePtr<FrameGraphNode>> sortQueue;
	for (auto& [k, v] : nodesInMap)
	{
		counterMap[k] = v.size();
		if (v.empty()) {
			sortQueue.push(k);
		}
	}

	uint32_t levelCounter = sortQueue.size();

	while (!sortQueue.empty())
	{
		auto front = sortQueue.front();
		sortQueue.pop();
		levelCounter--;

		printf("%s\n", front->name.c_str());

		auto& outs = nodesOutMap[front];
		for (auto& out : outs)
		{
			counterMap[out]--;
			if (counterMap[out] == 0) {
				sortQueue.push(out);
			}
		}

		if (!levelCounter) {
			printf("  ------------- \n");
			levelCounter = sortQueue.size();
		}
	}

}