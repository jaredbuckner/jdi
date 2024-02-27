// File: jdi_widget.cpp
// ----
// Base widget class.  All the widgety things.

#include "jdi.hpp"

namespace jdi {

  Widget::Widget() :
    _isEnabled(true),
    _isVisible(true),
    _padN(0),
    _padS(0),
    _padE(0),
    _padW(0),
    _minW(0),
    _minH(0),
    _anchors(JDI_NONE),
    _drawRect(),
    _self(),
    _parent() {
  }

  bool Widget::setParent(widget_ptr parent) {
    engine_ptr engine = Engine::getEngine();
    widget_ptr self = _self.lock();
    
    if(_parent.lock() || engine->getWindow(self)) {
      return(false); // Can't reparent
    }
    
    _parent = parent;

    renderer_ptr renderer = engine->getRenderer(self);
    if(renderer) {
      for(widget_ptr iter = getFirstPreOrderDFS();
          iter; iter = getNextPreOrderDFS(iter)) {
        iter->onRenderUpdate(renderer);
      }
    }
    
    return(true);
  }
  
  Widget::~Widget() {}
  
  int Widget::getPadding(direction_type direction) const {
    return((direction & JDI_N ? _padN : 0) +
           (direction & JDI_S ? _padS : 0) +
           (direction & JDI_E ? _padE : 0) +
           (direction & JDI_W ? _padW : 0));
  }

  void Widget::setPadding(direction_type direction,
                          int size) {
    if(direction & JDI_N) _padN = size;
    if(direction & JDI_S) _padS = size;
    if(direction & JDI_E) _padE = size;
    if(direction & JDI_W) _padW = size;
  }

  void Widget::setDrawRect(const SDL_Rect* boundingRect) {
    int width  = _padE + _padW + _minW;
    int height = _padN + _padS + _minH;

    int extraW = boundingRect->w > width  ? boundingRect->w - width  : 0;
    int extraH = boundingRect->h > height ? boundingRect->h - height : 0;
    
    _drawRect.x
      = _anchors & JDI_W ? boundingRect->x + _padE           // EW stretch or W anchor
      : _anchors & JDI_E ? boundingRect->x + _padE + extraW  // E anchor
      : boundingRect->x + _padE + extraW/2;                  // no anchor (centered)
    
    _drawRect.y
      = _anchors & JDI_N ? boundingRect->y + _padN           // NS stretch or N anchor
      : _anchors & JDI_S ? boundingRect->y + _padN + extraH  // S anchor
      : boundingRect->y + _padN + extraH/2;                  // no anchor (centered)

    _drawRect.w
      = (_anchors & JDI_EW) == JDI_EW ? _minW + extraW : _minW;

    _drawRect.h
      = (_anchors & JDI_NS) == JDI_NS ? _minH + extraH : _minH;
  }

  void Widget::onRenderUpdate(renderer_ptr renderer) {}

  void Widget::onDraw(renderer_ptr renderer) {}

  void Widget::onResize(renderer_ptr renderer) {}

  bool Widget::onTakeFocus(renderer_ptr renderer) { return(false); }

  void Widget::onLoseFocus(renderer_ptr renderer) {}

  bool Widget::onEvent(renderer_ptr renderer,
                       SDL_Event* event) { return(false); }

  bool Widget::hasChildren() const { return(false); }

  bool Widget::hasChild(widget_ptr child) const { return(false); }

  widget_ptr Widget::getFirstChild(widget_ptr prune) const { return(nullptr); }

  widget_ptr Widget::getNextChild(widget_ptr child,
                                  widget_ptr prune) const { return(nullptr); }

  widget_ptr Widget::getFirstPreOrderDFS(widget_ptr prune) const {
    widget_ptr self = _self.lock();
    return(self == prune ? widget_ptr{} : self);
  }

  widget_ptr Widget::getNextPreOrderDFS(widget_ptr iter, widget_ptr prune) const {
    if(iter == nullptr) return(getFirstPreOrderDFS(prune));

    widget_ptr self = _self.lock();
    widget_ptr last = nullptr;

    while(iter != nullptr && iter != self) {
      widget_ptr child = iter->getNextChild(last, prune);
      if(child != nullptr) return(child);
      
      last = iter;
      iter = iter->getParent();

    }
    return(nullptr);
  }

  widget_ptr Widget::getFirstPostOrderDFS(widget_ptr prune) const {
    widget_ptr iter = _self.lock();
    if(iter == prune) return(nullptr);
    
    widget_ptr child = getFirstChild(prune);
    while(child != nullptr) {
      iter = child;
      child = getFirstChild(prune);
    }

    return(iter);    
  }

  widget_ptr Widget::getNextPostOrderDFS(widget_ptr iter, widget_ptr prune) const {    
    if(iter == nullptr) return(getFirstPostOrderDFS(prune));

    widget_ptr self = _self.lock();
    if(iter == self) return(nullptr);
    
    widget_ptr parent = iter->getParent();
    if(parent == nullptr) return(nullptr);  // This shouldn't happen
    iter = parent->getNextChild(iter, prune);
    if(iter == nullptr) return(parent);
    return(iter->getFirstPostOrderDFS(prune));

  }
  
  widget_ptr Widget::getRoot() const {
    widget_ptr reply = getSelf();
    widget_ptr iter = reply->getParent();
    while(iter) {
      reply = iter;
      iter = reply->getParent();
    }
    return(reply);
  }
  
} // end namespace jdi
