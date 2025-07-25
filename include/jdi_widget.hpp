// File: jdi_widget.hpp
// ----
// Widgets widge but they don't fall down


namespace jdi {
  ////
  // A widget for drawing and responding to events
  ////
  class Widget {
  private:
    bool _isEnabled;  // Does this widget receive events?
    bool _isVisible;  // Does this widget appear when rendered?

    // Minimum padding in each direction.
    int _padN;
    int _padS;
    int _padE;
    int _padW;

    // Minimum size
    int _minW;
    int _minH;

    // Desired anchor directions.  Determines whether additional space is
    // allocated to the widget draw area or to the surrounding padding.
    direction_type _anchors;

    // The sized boundary and drawing space assigned for this widget, if
    // visible
    SDL_Rect _boundRect;
    SDL_Rect _drawRect;

    // Hierarchy is useful
    widget_ptr::weak_type _self;
    widget_ptr::weak_type _parent;
    
  protected:
    Widget();
    void setSelf(widget_ptr self);

    // A child can only be claimed once.  Child must exist.  Widgets which are already a
    // window root cannot become someone else's child.
    bool claimChild(widget_ptr child);
    
  public:
    virtual ~Widget();
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    bool isVisible() const;
    void setVisible(bool visible);
    
    int getPadding(direction_type direction) const;  // Sum of direction paddings
    // Set all requested direction pads to be the given size
    void setPadding(direction_type direction,
                    int size);
    
    int getMinW() const;
    int getMinH() const;
    void getMinSize(int& w, int& h) const;
    
    void setMinW(int w);
    void setMinH(int h);
    void setMinSize(int w, int h);

    direction_type getAnchors() const;
    void setAnchors(direction_type anchors);

    const SDL_Rect* getDrawRect() const;    
    // From the bounding box, generate a draw rect which obeys the padding,
    // sizing, and anchors.
    void setDrawRect(const SDL_Rect* boundingRect);

    // All three bools are true if the point is inside the drawRect
    bool isInside(const SDL_Point* absPtr) const;
    bool rel2Abs(const SDL_Point* relPtr,
                 SDL_Point* absPtr) const;
    bool abs2Rel(const SDL_Point* absPtr,
                 SDL_Point* relPtr) const;
    
    //
    // Handlers.  Overload as needed.
    //

    // When the renderer has been updated.  Can be nullptr if there is no
    // longer a renderer.  This is a good time to set up any textures and get
    // them ready for use.
    //
    // You are NOT responsible for propagating this to your children
    virtual void onRenderUpdate(renderer_ptr renderer);

    // When it's time to draw something.  Renderer is always defined.  Your
    // DrawRect has already been set.  Have at it!
    //
    // You ARE responsible for propagating this to your children
    virtual void onDraw(renderer_ptr renderer);

    // This lets you know that your size has (potentially) been changed, in
    // case you need to make changes to any of your data structures.  Don't
    // actually render anything, though -- you'll get an onDraw if that is
    // needed.  Your new DrawRect size is already ready, already.  Renderer is
    // always defined.
    //
    // You ARE responsible for propagating this to your children after you set
    // their new bounding boxes.
    virtual void onResize(renderer_ptr renderer);

    // You are now the most important widget in the world.  Return true to
    // accept the honor, false to pass it on.  (You don't get a LoseFocus if
    // you pass.)
    virtual bool onTakeFocus(renderer_ptr renderer);
    
    // You are no longer the most important widget in the world.  You don't get
    // a say in that.
    virtual void onLoseFocus(renderer_ptr renderer);

    // Something happened.  You can do something with the information.  Return
    // true if the event should stop propagating, false otherwise.
    //
    // You are NOT responsible for propagating this to your children.
    virtual bool onEvent(renderer_ptr renderer,
                         SDL_Event* event);
    
    ////
    // Self -- For canonicalization
    ////
    widget_ptr getSelf() const;
    
    ////
    // Children -- Many widgets don't have these, so don't sweat it.
    ////
    virtual bool hasChildren() const;
    virtual bool hasChild(widget_ptr child) const;

    // Iterate through the child widgets, skipping the prune widget if provided.
    virtual widget_ptr getFirstChild(widget_ptr prune=nullptr) const;
    virtual widget_ptr getNextChild(widget_ptr child,
                                    widget_ptr prune=nullptr) const;

    // Pre-order and Post-order depth-first traversal of the tree
    widget_ptr getFirstPreOrderDFS(widget_ptr prune=nullptr) const;
    widget_ptr getNextPreOrderDFS(widget_ptr iter, widget_ptr prune=nullptr) const;
    widget_ptr getFirstPostOrderDFS(widget_ptr prune=nullptr) const;
    widget_ptr getNextPostOrderDFS(widget_ptr iter, widget_ptr prune=nullptr) const;
    
    ////
    // Parent -- If a widget has a parent, then parent->hasChild(me) must be
    // true.
    ////
    widget_ptr getParent() const;
    widget_ptr getRoot() const;  // As far up the tree as you can go
    
  }; // end class Widget


  inline void Widget::setSelf(widget_ptr self) { _self = self; }
  inline bool Widget::isEnabled() const { return(_isEnabled); }
  inline void Widget::setEnabled(bool enabled) { _isEnabled = enabled; }

  inline bool Widget::isVisible() const { return(_isVisible); }
  inline void Widget::setVisible(bool visible) { _isVisible = visible; }

  inline int Widget::getMinW() const { return(_minW); }
  inline int Widget::getMinH() const { return(_minH); }
  inline void Widget::getMinSize(int& w, int& h) const { w = _minW; h = _minH; }

  inline void Widget::setMinW(int w) { _minW = w; }
  inline void Widget::setMinH(int h) { _minH = h; }
  inline void Widget::setMinSize(int w, int h) { _minW = w; _minH = h; }

  inline direction_type Widget::getAnchors() const { return(_anchors); }
  inline void Widget::setAnchors(direction_type anchors) { _anchors = anchors; }

  inline const SDL_Rect* Widget::getDrawRect() const { return(&_drawRect); }

  inline bool Widget::isInside(const SDL_Point* absPtr) const {
    return(SDL_PointInRect(absPtr, &_drawRect) == SDL_TRUE);
  }

  inline bool Widget::rel2Abs(const SDL_Point* relPtr,
                              SDL_Point* absPtr) const {
    absPtr->x = relPtr->x + _drawRect.x;
    absPtr->y = relPtr->y + _drawRect.y;

    return(isInside(absPtr));
  }
  
  inline bool Widget::abs2Rel(const SDL_Point* absPtr,
                              SDL_Point* relPtr) const {
    relPtr->x = absPtr->x - _drawRect.x;
    relPtr->y = absPtr->y - _drawRect.y;

    return(isInside(absPtr));
  }
  
  inline widget_ptr Widget::getSelf() const { return(_self.lock()); }
  inline widget_ptr Widget::getParent() const { return(_parent.lock()); }

} // end namespace jdi
