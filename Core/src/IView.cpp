//
// Created by Mario on 2023/1/18.
//

#include <IView.h>

IView::IView(IntrusivePtr<SwapChain> swapChain) : vulkanContext(swapChain->context), swapChain(swapChain)
{

}

IView::~IView()
{
}
