#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>

#include <VulkanContext.h>
#include <RenderPass.h>
#include <FrameGraphNode.h>

class FrameGraph : public IntrusiveCounter<FrameGraph>
{
public:
	FrameGraph(IntrusivePtr<VulkanContext>& context);
	~FrameGraph();

	template<class T, class ...Args>
	IntrusivePtr<T> CreateNode(std::string name, Args&& ... args)
	{
		auto p = gmetal::make_intrusive<T>(std::forward<Args>(args)...);
		p->fg = this;
		p->name = name;
		this->nodesInMap[p];
		this->nodesOutMap[p];
		return p;
	}

	template<class T, class ...Args>
	IntrusivePtr<T> CreateImage(std::string name, Args&& ... args)
	{
		auto image = CreateNode<T>(name, std::forward<Args>(args)...);
		imageNodes.push_back(image);
		return image;
	}

	template<class T, class ...Args>
	IntrusivePtr<T> CreatePass(std::string name, Args&& ... args)
	{
		auto pass = CreateNode<T>(name, std::forward<Args>(args)...);
		passNodes.push_back(pass);
		return pass;
	}

	void Link(IntrusivePtr<FrameGraphNode>&& from, IntrusivePtr<FrameGraphNode>&& to);

	void Compile();

	// alloc image for imageResourceNode
	// alloc attachment image views
	// create VkFramebuffer
	// create descriptor pool
	// alloc descriptor sets for each pass
	void ResolveResource(VkExtent2D extent);

	// bind resources && drawcall
	void Execute();

	IntrusivePtr<VulkanContext>& Context();
	
private:
	IntrusivePtr<VulkanContext>& context;
	std::vector<IntrusivePtr<FrameGraphNode>> imageNodes;
	std::vector<IntrusivePtr<FrameGraphNode>> passNodes;
	std::unordered_map<IntrusivePtr<FrameGraphNode>, std::vector<IntrusivePtr<FrameGraphNode>>> nodesInMap;
	std::unordered_map<IntrusivePtr<FrameGraphNode>, std::vector<IntrusivePtr<FrameGraphNode>>> nodesOutMap;

	void TopoSort();
	void Cull();
	void TraceBack(const IntrusivePtr<FrameGraphNode>& node);

	void BuildRenderPass();

	// helper for building VkAttachmentDescriptions
	std::vector<VkAttachmentDescription> attachmentsDescriptions;
	// store toposort result
	std::vector<std::vector<IntrusivePtr<FrameGraphNode>>> levelResult;

	IntrusivePtr<RenderPass> renderPass;

	void printNode() {
		for (auto n : passNodes) {
			spdlog::info("{} count: {}", n->name.c_str(), n->refCount);
		}

		for (auto n : imageNodes) {
			spdlog::info("{} count: {}", n->name.c_str(), n->refCount);
		}
	}
};