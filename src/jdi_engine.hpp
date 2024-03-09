// File: jdi_engine.hpp
// ----
// Declarations for the engine class

namespace jdi {
  ////
  // JDI event codes
  ////
  typedef enum {
    JDI_ANIMATE = 1,
  } jdi_event_type;
  
  
  ////
  // The engine.  Its job is to build and tear-down SDL itself, to handle the
  // eventloop, and to associate a root widget with each window.  There's only
  // at most one at a time, so even if you attempt to make more than one, you
  // will always get the same one.  Mostly you should build one at the start of
  // your program and let it clean up SDL at the end.
  ////
  class Engine {
  private:
    struct window_datum_type {
      window_ptr             window;
      renderer_ptr           renderer;
      widget_ptr             root;
      widget_ptr::weak_type  focus;
      SDL_Rect               bbox;
      Color                  bgColor;
      bool                   isBorderlessFS;
      bool                   willResize;
      bool                   willUpdate;
    };
    
    typedef std::vector<window_datum_type> window_data_type;
    typedef std::vector<joystick_ptr> joystick_seq_type;

    window_data_type _windowData;
    joystick_seq_type _joystickData;
    bool              _joysticksEnabled;

    Uint32       _ticksPerFrame;
    SDL_TimerID  _animateTimer;

    void addJoystick(Uint32 deviceIndex);
    void removeJoystick(Uint32 instanceID);
    
    window_datum_type* getDataByWindow(window_ptr window);
    const window_datum_type* getDataByWindow(window_ptr window) const;

    window_datum_type* getDataByWindowID(Uint32 windowID);
    const window_datum_type* getDataByWindowID(Uint32 windowID) const;

    window_datum_type* getDataByWidget(widget_ptr widget);
    const window_datum_type* getDataByWidget(widget_ptr widget) const;
    
    void updateRenderer(window_datum_type* dataPtr);

    void setFullscreen(window_datum_type* dataPtr,
                       bool enabled);
    
    void toggleFullscreen(window_datum_type* dataPtr);
    
    void resizeWidgets(window_datum_type* dataPtr);
    void updateWidgets(window_datum_type* dataPtr);
    
    void startAnimateCallback();
    static Uint32 animateCallback(Uint32 interval, void* dummy);
    void removeAnimateCallback();

    window_datum_type* getEventFocus(const SDL_Event& event);

    bool sendEvent(window_datum_type* dataPtr,
                   SDL_Event* event);
    
    bool _willExit;
    
  protected:
    Engine();
    
  public:
    virtual ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    bool areJoysticksEnabled() const;
    void enableJoysticks(bool enable);
    
    bool hasWindows() const;
    bool hasWindow(window_ptr window) const;
    window_ptr getFirstWindow() const;
    window_ptr getNextWindow(window_ptr window) const;

    window_ptr createWindow(const char* title,
                            int x=SDL_WINDOWPOS_UNDEFINED,
                            int y=SDL_WINDOWPOS_UNDEFINED,
                            int w=800, int h=600,
                            Uint32 flags=SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    void removeWindow(window_ptr window);
    
    window_ptr getWindow(widget_ptr widget) const;

    const Color& getWindowBGColor(window_ptr window) const;
    void         setWindowBGColor(window_ptr window,
                                  const Color& color);
    
    renderer_ptr getRenderer(window_ptr window) const;
    renderer_ptr getRenderer(widget_ptr widget) const;
    
    widget_ptr   getRoot(window_ptr window) const;
    void         setRoot(window_ptr window, widget_ptr widget);
    
    widget_ptr   getFocus(window_ptr window) const;
    void         setFocus(widget_ptr widget);
    void         clearFocus(window_ptr window);

    Uint32       getFrameRate() const;                // Ticks-per-frame, all windows share
    void         setFrameRate(Uint32 ticksPerFrame);  // 0 to disable all animation
    
    void         requestResize(window_ptr window);
    void         requestResize(widget_ptr widget);
    void         requestResizeAll();
    
    void         requestUpdate(window_ptr window);
    void         requestUpdate(widget_ptr widget);
    void         requestUpdateAll();

    void         setFullscreen(window_ptr window,
                               bool enabled);
    void         setFullscreen(widget_ptr widget,
                               bool enabled);
    void         toggleFullscreen(window_ptr window);
    void         toggleFullscreen(widget_ptr widget);
    
    void         requestExit();    

    void         mainLoop();  // Do the mainloop until someone requests an exit
    
    static Uint32     getJDIEventType();
    static engine_ptr getEngine();
    
  }; // end class Engine



  inline void Engine::toggleFullscreen(Engine::window_datum_type* dataPtr) {
    if(dataPtr != nullptr) setFullscreen(dataPtr, !dataPtr->isBorderlessFS);
  }
  
  inline bool Engine::areJoysticksEnabled() const { return(_joysticksEnabled); }
  
  inline bool Engine::hasWindow(window_ptr window) const {
    auto dataPtr = getDataByWindow(window);

    return(dataPtr == nullptr ? false : true);
  }
  
  inline bool Engine::hasWindows() const { return(!_windowData.empty()); }
  inline window_ptr Engine::getFirstWindow() const {
    return(_windowData.empty() ? window_ptr() : _windowData.front().window);
  }

  inline window_ptr Engine::getWindow(widget_ptr widget) const {
    auto dataPtr = getDataByWidget(widget);

    return(dataPtr == nullptr ? window_ptr() : dataPtr->window);
  }

  inline renderer_ptr Engine::getRenderer(window_ptr window) const {
    auto dataPtr = getDataByWindow(window);

    return(dataPtr == nullptr ? renderer_ptr() : dataPtr->renderer);
  }

  inline renderer_ptr Engine::getRenderer(widget_ptr widget) const {
    auto dataPtr = getDataByWidget(widget);

    return(dataPtr == nullptr ? renderer_ptr() : dataPtr->renderer);
  }

  inline widget_ptr Engine::getRoot(window_ptr window) const {
    auto dataPtr = getDataByWindow(window);

    return(dataPtr == nullptr ? widget_ptr() : dataPtr->root);
  }

  inline widget_ptr Engine::getFocus(window_ptr window) const {
    auto dataPtr = getDataByWindow(window);

    return(dataPtr == nullptr ? widget_ptr() : dataPtr->focus.lock());
  }
  
  inline Uint32 Engine::getFrameRate() const { return(_ticksPerFrame); }

  inline void Engine::setFrameRate(Uint32 ticksPerFrame) {
    _ticksPerFrame = ticksPerFrame;
  }
    
  inline void Engine::requestResize(window_ptr window) {
    auto dataPtr = getDataByWindow(window);

    if(dataPtr != nullptr) { dataPtr->willResize = true; }
  }

  inline void Engine::requestResize(widget_ptr widget) {
    auto dataPtr = getDataByWidget(widget);

    if(dataPtr != nullptr) { dataPtr->willResize = true; }
  }

  inline void Engine::requestUpdate(window_ptr window) {
    auto dataPtr = getDataByWindow(window);

    if(dataPtr != nullptr) { dataPtr->willUpdate = true; }
  }

  inline void Engine::requestUpdate(widget_ptr widget) {
    auto dataPtr = getDataByWidget(widget);

    if(dataPtr != nullptr) { dataPtr->willUpdate = true; }
  }
  
  inline void Engine::setFullscreen(window_ptr window,
                                    bool enabled) {
    setFullscreen(getDataByWindow(window), enabled);
  }
  
  inline void Engine::setFullscreen(widget_ptr widget,
                                    bool enabled) {
    setFullscreen(getDataByWidget(widget), enabled);
  }
  
  inline void Engine::toggleFullscreen(window_ptr window) {
    toggleFullscreen(getDataByWindow(window));
  }

  inline void Engine::toggleFullscreen(widget_ptr widget) {
    toggleFullscreen(getDataByWidget(widget));
  }
  
  inline void Engine::requestExit() { _willExit = true; }
  
}
