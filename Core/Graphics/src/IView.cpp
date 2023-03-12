//
// Created by Mario on 2023/1/18.
//

#include <IView.h>

IView::IView(IntrusivePtr<VulkanContext> context) : context(context){

}

IView::~IView()
{
}

IntrusivePtr<SwapChain> IView::GetSwapChain()
{
	return this->swapChain;
}

void IView::SetSwapChain(IntrusivePtr<SwapChain> swapChain) { this->swapChain = swapChain; }