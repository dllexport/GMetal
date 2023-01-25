#pragma once

#include <vector>
#include <IView.h>

class Renderer {
public:
	void AddView(IntrusivePtr<IView>& view) {
		views.push_back(view);
	}

	void StartFrame();

private:
	std::vector<IntrusivePtr<IView>> views;
};