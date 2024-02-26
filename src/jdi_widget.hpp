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

    // Can only be set once.  Parent must exist.  Widgets which are already a
    // window root cannot be reparented.
    bool setParent(widget_ptr parent);
    
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
    
    //
    // Handlers.  Overload as needed.
    //

    // When the renderer has been updated.  Can be nullptr if there is no
    // longer a renderer.  This is a good time to set up any textures and get
    // them ready for use.
    virtual void onRenderUpdate(renderer_ptr renderer);

    // When it's time to draw something.  Renderer is always defined.  Your
    // DrawRect has already been set.  Have at it!
    virtual void onDraw(renderer_ptr renderer);

    // This lets you know that your size has (potentially) been changed, in
    // case you need to make changes to any of your data structures.  Don't
    // actually render anything, though -- you'll get an onDraw if that is
    // needed.  Your new DrawRect size is already ready, already.  Renderer is
    // always defined.
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
    virtual widget_ptr getFirstChild() const;
    virtual widget_ptr getNextChild(widget_ptr child) const;
    virtual widget_ptr getFirstDescendant() const;
    virtual widget_ptr getNextDescendant(widget_ptr child) const;
    
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
  inline widget_ptr Widget::getSelf() const { return(_self.lock()); }
  inline widget_ptr Widget::getParent() const { return(_parent.lock()); }

} // end namespace jdi
