//
// Created by Mario on 2023/1/18.
//

#include <IView.h>

IView::IView(IntrusivePtr<SwapChain> swapChain) : context(swapChain->context), swapChain(swapChain)
{

}

IView::~IView()
{
}

IntrusivePtr<SwapChain> IView::GetSwapChain()
{
	return this->swapChain;
}