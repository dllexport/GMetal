#pragma once

#include <vector>
#include <IView.h>
#include <Drawable.h>
#include <unordered_map>
#include <unordered_set>
#include <RenderPass.h>

class Renderer : public IntrusiveCounter<Renderer> {
public:
	void AddView(IntrusivePtr<IView>& view);
	void SetDefaultRenderPass(IntrusivePtr<RenderPass> renderPass);
	void AddDrawable(IntrusivePtr<Drawable> drawable, IntrusivePtr<IView>& view, IntrusivePtr<RenderPass> renderPass = nullptr);
	void StartFrame();
private:
	using DrawableMap = std::unordered_map<IntrusivePtr<RenderPass>, std::vector<IntrusivePtr<Drawable>>>;
	std::unordered_map<IntrusivePtr<IView>, DrawableMap> views;

	uint64_t frameNumber = 0;

	IntrusivePtr<RenderPass> defaultRenderPass;

	std::vector<VkCommandBuffer> drawCmdBuffers;

	void Draw(IntrusivePtr<Drawable> drawable);
	void BuildFrameCommandBuffer(const IntrusivePtr<IView>& view, const IntrusivePtr<RenderPass>& renderPass, std::vector<IntrusivePtr<Drawable>>& drawables);
};