// File: jdi_engine.hpp
// ----
// Declarations for the engine class

namespace jdi {
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

    window_data_type _windowData;

    window_datum_type* getDataByWindow(window_ptr window);
    const window_datum_type* getDataByWindow(window_ptr window) const;

    window_datum_type* getDataByWindowID(Uint32 windowID);
    const window_datum_type* getDataByWindowID(Uint32 windowID) const;

    window_datum_type* getDataByWidget(widget_ptr widget);
    const window_datum_type* getDataByWidget(widget_ptr widget) const;
    
    bool _willExit;
    
  protected:
    Engine();
    
  public:
    virtual ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

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

    void         requestResize(window_ptr window);
    void         requestResize(widget_ptr widget);
    void         requestResizeAll();
    
    void         requestUpdate(window_ptr window);
    void         requestUpdate(widget_ptr widget);
    void         requestUpdateAll();
    
    void         requestExit();
    
    void         mainLoop();  // Do the mainloop until someone requests an exit
    
    
    static engine_ptr getEngine();
    
  }; // end class Engine



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
  
  inline void Engine::requestExit() { _willExit = true; }


  
}
