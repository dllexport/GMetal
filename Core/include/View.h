#pragma once

#include <Node.h>

class IView;
class View : public IntrusiveCounter<View> {
public:
    View();
    void SetRootNode(IntrusivePtr<Node> node);
    void StartFrame();
private:
    IntrusivePtr<IView> graphicView;
    IntrusivePtr<Node> rootNode;
};