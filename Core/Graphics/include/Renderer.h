#pragma once

#include <vector>
#include <IView.h>
#include <Drawable.h>
#include <unordered_map>
#include <unordered_set>
#include <FrameGraph.h>
#include <VulkanContext.h>

class Renderer : public IntrusiveCounter<Renderer> {
public:
	Renderer(IntrusivePtr<VulkanContext>& context);
	void AddView(IntrusivePtr<IView>& view);
    void AddDrawable(IntrusivePtr<Drawable> drawable,
                               IntrusivePtr<IView>& view,
                               IntrusivePtr<FrameGraph> fg);
    void StartFrame();

private:
	IntrusivePtr<VulkanContext>& context;

	using DrawableMap = std::unordered_map<IntrusivePtr<FrameGraph>, std::vector<IntrusivePtr<Drawable>>>;
	std::unordered_map<IntrusivePtr<IView>, DrawableMap> views;

	uint64_t frameNumber = 0;

	void InitDefaultFrameGraph();
	IntrusivePtr<FrameGraph> defaultFg;

	std::vector<VkCommandBuffer> drawCmdBuffers;

	void Draw(IntrusivePtr<Drawable> drawable);
};