// File: jdi_engine.cpp
// ----
// Handle SDL init/mainloop/destroy; and other goodies.


#include "jdi.hpp"

namespace jdi {

  engine_ptr::weak_type& getSingletonEngine() {
    static engine_ptr::weak_type _hiddenEngine;

    return(_hiddenEngine);
  }

  void Engine::addJoystick(Uint32 deviceIndex) {
    if(_joysticksEnabled) {
      _joystickData.push_back(sdl_shared(SDL_JoystickOpen(deviceIndex)));
    }
  }

  void Engine::removeJoystick(Uint32 instanceID) {
    SDL_Joystick* jptr = SDL_JoystickFromInstanceID(instanceID);
    auto iter = _joystickData.begin();
    while(iter != _joystickData.end()) {
      if(iter->get() == jptr) {
        iter = _joystickData.erase(iter);
      } else {
        ++iter;
      }
    }
  }
  
  Engine::window_datum_type* Engine::getDataByWindow(window_ptr window) {
    for(auto& data : _windowData) {
      if(data.window == window) return (&data);
    }
    return(nullptr);
  }

  const Engine::window_datum_type* Engine::getDataByWindow(window_ptr window) const {
    for(auto& data : _windowData) {
      if(data.window == window) return (&data);
    }
    return(nullptr);
  }

  Engine::window_datum_type* Engine::getDataByWindowID(Uint32 windowID) {
    for(auto& data : _windowData) {
      if(SDL_GetWindowID(data.window.get()) == windowID) return(&data);
    }
    return(nullptr);
  }

  const Engine::window_datum_type* Engine::getDataByWindowID(Uint32 windowID) const {
    for(auto& data : _windowData) {
      if(SDL_GetWindowID(data.window.get()) == windowID) return(&data);
    }
    return(nullptr);
  }

  Engine::window_datum_type* Engine::getDataByWidget(widget_ptr widget) {
    widget_ptr root = widget->getRoot();
    for(auto& data : _windowData) {
      if(data.root == root) return (&data);
    }
    return(nullptr);
  }

  const Engine::window_datum_type* Engine::getDataByWidget(widget_ptr widget) const {
    widget_ptr root = widget->getRoot();
    for(auto& data : _windowData) {
      if(data.root == root) return (&data);
    }
    return(nullptr);
  }

  void Engine::updateRenderer(Engine::window_datum_type* dataPtr) {
    dataPtr->renderer.reset();  // Destroy the old renderer            
    dataPtr->renderer
      = sdl_shared(SDL_CreateRenderer(dataPtr->window.get(),
                                      -1,   // First matching
                                      0));  // HW Accellerator requested but not required.
    
    safely(SDL_GetRendererOutputSize(dataPtr->renderer.get(),
                                     &(dataPtr->bbox.w),
                                     &(dataPtr->bbox.h)));

    SDL_SetRenderDrawBlendMode(dataPtr->renderer.get(),
                               SDL_BLENDMODE_BLEND);
    
    if(dataPtr->root) {
      for(widget_ptr iter = dataPtr->root->getFirstPreOrderDFS();
          iter; iter = dataPtr->root->getNextPreOrderDFS(iter)) {
        iter->onRenderUpdate(dataPtr->renderer);
      }      
    }

    dataPtr->willResize = true;
    dataPtr->willUpdate = true;
  }
  
  void Engine::setFullscreen(Engine::window_datum_type* dataPtr,
                             bool enabled) {
    if(dataPtr != nullptr) {
      dataPtr->isBorderlessFS = enabled;
      
      if(dataPtr->isBorderlessFS) {
        SDL_SetWindowFullscreen(dataPtr->window.get(),
                                SDL_WINDOW_FULLSCREEN_DESKTOP);
      } else {
        SDL_SetWindowFullscreen(dataPtr->window.get(), 0);      
      }
    }
  }

  void Engine::resizeWidgets(window_datum_type* dataPtr) {
    if(dataPtr->willResize && dataPtr->root && dataPtr->root->isVisible()) {      
      dataPtr->root->setDrawRect(&(dataPtr->bbox));
      dataPtr->root->onResize(dataPtr->renderer);
    }
    dataPtr->willResize = false;
  }

  void Engine::updateWidgets(window_datum_type* dataPtr) {
    if(dataPtr->willUpdate) {
      SDL_SetRenderDrawColor(dataPtr->renderer.get(),
                             dataPtr->bgColor.r,
                             dataPtr->bgColor.g,
                             dataPtr->bgColor.b,
                             dataPtr->bgColor.a);
      safely(SDL_RenderClear(dataPtr->renderer.get()));            
      if(dataPtr->root && dataPtr->root->isVisible()) {
        dataPtr->root->onDraw(dataPtr->renderer);
      }
      SDL_RenderPresent(dataPtr->renderer.get());
    }
    dataPtr->willUpdate = false;
  }
  
  void Engine::startAnimateCallback() {
    if(_animateTimer == 0 &&
       _ticksPerFrame != 0) {
      _animateTimer = SDL_AddTimer(_ticksPerFrame,
                                   *animateCallback,
                                   this);
    }
  }
  
  Uint32 Engine::animateCallback(Uint32 interval, void* engine) {
    // We can't actually draw on this thread.  We'll push a special event onto
    // the event queue and let the engine handle it when the queue makes it
    // around.
    static Uint32 jdiEventType = Engine::getJDIEventType();
    
    SDL_Event event;
    event.type = jdiEventType;
    event.user.windowID = 0;
    event.user.code = JDI_ANIMATE;
    event.user.data1 = nullptr;
    event.user.data2 = nullptr;
    SDL_PushEvent(&event);
    
    return(static_cast<Engine*>(engine)->_ticksPerFrame);
  }
  
  void Engine::removeAnimateCallback() {
    if(_animateTimer != 0) {
      SDL_RemoveTimer(_animateTimer);
      _animateTimer = 0;
    }
  }
    

  // If the event focuses on a particular window, figure that out and return
  // the associated datum
  Engine::window_datum_type* Engine::getEventFocus(const SDL_Event& event) {
    switch(event.type) {
    case SDL_WINDOWEVENT:
      return(getDataByWindowID(event.window.windowID));
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      return(getDataByWindowID(event.key.windowID));
    case SDL_TEXTEDITING:
      return(getDataByWindowID(event.edit.windowID));
    case SDL_TEXTEDITING_EXT:
      return(getDataByWindowID(event.editExt.windowID));
    case SDL_TEXTINPUT:
      return(getDataByWindowID(event.text.windowID));
    case SDL_MOUSEMOTION:
      return(getDataByWindowID(event.motion.windowID));
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      return(getDataByWindowID(event.button.windowID));
    case SDL_MOUSEWHEEL:
      return(getDataByWindowID(event.wheel.windowID));
    case SDL_FINGERMOTION:
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
      return(getDataByWindowID(event.tfinger.windowID));
    case SDL_DROPBEGIN:
    case SDL_DROPFILE:
    case SDL_DROPTEXT:
    case SDL_DROPCOMPLETE:
      return(getDataByWindowID(event.drop.windowID));
    default:
      return(event.type < SDL_USEREVENT ? nullptr : getDataByWindowID(event.user.windowID));
    }
  }


  bool Engine::sendEvent(Engine::window_datum_type* dataPtr,
                         SDL_Event* eventPtr) {
    bool isHalted = false;    
    if(dataPtr->root) {      
      widget_ptr focus = dataPtr->focus.lock();
      
      // Handle focus tree first
      if(focus != nullptr) {
        for(widget_ptr iter = focus->getFirstPostOrderDFS();
            isHalted == false && iter; iter = focus->getNextPostOrderDFS(iter)) {
          if(iter->isEnabled()) {
            isHalted = iter->onEvent(dataPtr->renderer,
                                     eventPtr);
          }
        }
      }

      // Handle remaining tree with focus pruned
      for(widget_ptr iter = dataPtr->root->getFirstPostOrderDFS(focus);
          isHalted == false && iter; iter = dataPtr->root->getNextPostOrderDFS(iter, focus)) {
        if(iter->isEnabled()) {
          isHalted = iter->onEvent(dataPtr->renderer,
                                   eventPtr);
        }
      }
    }

    return(isHalted);
  }
  
  
  Engine::Engine() :
    _joysticksEnabled(false)
  {    
    if(getSingletonEngine().lock()) {
      throw(std::logic_error("Cannot have multiple simultaneous JDI Engines!"));
    }

    safely(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_GAMECONTROLLER));
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) throw(std::runtime_error("Cannot initialize the image libraries!"));
    safely(TTF_Init());
  }

  Engine::~Engine() {
    removeAnimateCallback();  // No reason to animate anything now, is there?
    _windowData.clear();  // Clear window data _before_ shutting down SDL
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
  }
  
  engine_ptr Engine::getEngine() {
    engine_ptr reply = getSingletonEngine().lock();
    if(!reply) {
      reply = engine_ptr(new Engine());
      getSingletonEngine() = reply;
    }
    return(reply);
  }

  void Engine::enableJoysticks(bool enable) {
    if(!enable) {
      _joystickData.clear();      
    } else if(_joystickData.empty()) {
      SDL_LockJoysticks();
      for(int devIdx = 0; devIdx < SDL_NumJoysticks(); ++devIdx) {
        _joystickData.push_back(sdl_shared(SDL_JoystickOpen(devIdx)));
      }
      SDL_UnlockJoysticks();
    }

    _joysticksEnabled = enable;
  }
  
  window_ptr Engine::getNextWindow(window_ptr window) const {
    if(!window) return(getFirstWindow());
    
    for(auto iter = _windowData.begin();
        iter != _windowData.end(); ++iter) {
      if(iter->window == window) {
        ++iter;
        return(iter == _windowData.end() ? window_ptr() : iter->window);
      }
    }
    return(window_ptr());
  }

  window_ptr Engine::createWindow(const char* title,
                                  int x, int y, int w, int h,
                                  Uint32 flags) {
    window_ptr window
      = sdl_shared(SDL_CreateWindow(title, x, y, w, h, flags));

    _windowData.push_back(window_datum_type{window});

    window_datum_type* dataPtr = &(_windowData.back());

    dataPtr->bbox.x = 0;
    dataPtr->bbox.y = 0;
    dataPtr->bgColor.set(255, 0, 255);

    updateRenderer(dataPtr);
    
    return(window);
  }

  void Engine::removeWindow(window_ptr window) {
    for(auto iter = _windowData.begin(); iter != _windowData.end(); ++iter) {
      if(iter->window == window) {
        _windowData.erase(iter);
        break;
      }
    }
  }

  const Color& Engine::getWindowBGColor(window_ptr window) const {
    static Color black(0, 0, 0);

    auto dataPtr = getDataByWindow(window);
    return(dataPtr == nullptr ? black : dataPtr->bgColor);
  }

  void Engine::setWindowBGColor(window_ptr window,
                                const Color& color) {
    auto dataPtr = getDataByWindow(window);
    if(dataPtr != nullptr) { dataPtr->bgColor = color; }
  }
  
  void Engine::setRoot(window_ptr window, widget_ptr widget) {
    if(widget) {
      if(widget->getParent()) throw(std::logic_error("You can't root a widget that has a parent!"));

      auto dataPtr = getDataByWidget(widget);
      if(dataPtr != nullptr) {
        if(dataPtr->window == window) {
          return;  // No-op
        }
        dataPtr->focus.reset();
        dataPtr->root.reset();        
      }
    }
    
    auto dataPtr = getDataByWindow(window);

    if(dataPtr != nullptr) {
      dataPtr->root = widget;

      if(widget) {
        for(widget_ptr child = widget->getFirstPreOrderDFS();
            child; child = widget->getNextPreOrderDFS(child)) {
          child->onRenderUpdate(dataPtr->renderer);
        }
      }
    }
  }  
  
  void Engine::setFocus(widget_ptr widget) {
    auto dataPtr = getDataByWidget(widget);

    if(dataPtr != nullptr) {
      widget_ptr oldFocus = dataPtr->focus.lock();
      
      if(oldFocus == widget) return;  // No-op
      
      if(oldFocus) { oldFocus->onLoseFocus(dataPtr->renderer); }
      
      widget_ptr tgt = widget;
      while(tgt && !tgt->onTakeFocus(dataPtr->renderer)) { tgt = tgt->getParent(); }
      
      dataPtr->focus = tgt;
    }
  }
  
  void Engine::clearFocus(window_ptr window) {
    auto dataPtr = getDataByWindow(window);
    
    if(dataPtr != nullptr) {
      widget_ptr oldFocus = dataPtr->focus.lock();
      
      if(oldFocus) { oldFocus->onLoseFocus(dataPtr->renderer); }
      
      dataPtr->focus.reset();
    }
  }

  void Engine::requestResizeAll() {
    for(auto& data : _windowData) {
      data.willResize = true;
    }
  }

  void Engine::requestUpdateAll() {
    for(auto& data : _windowData) {
      data.willUpdate = true;
    }
  }
  
  Uint32 Engine::getJDIEventType() {
    static Uint32 _jdiEventType = SDL_RegisterEvents(1);

    return(_jdiEventType);
  }
  
  void Engine::mainLoop() {
    const Uint32 jdiEventType = getJDIEventType();
    
    _willExit = false;
    
    while(!_willExit) {
      SDL_Event event;

      startAnimateCallback();
      if(1 != SDL_WaitEvent(&event)) throw(Error("SDL_WaitEvent"));

      switch(event.type) {
        
      case SDL_QUIT:
        _willExit = true;
        break;
        
      case SDL_KEYUP:
        if(event.key.keysym.sym == SDLK_F11) {
          auto dataPtr = getDataByWindowID(event.key.windowID);
          toggleFullscreen(dataPtr);          
        }
        break;
        
      case SDL_WINDOWEVENT:
        {
          switch(event.window.event) {
          case SDL_WINDOWEVENT_SHOWN:
          case SDL_WINDOWEVENT_EXPOSED:
            {
              auto dataPtr = getDataByWindowID(event.window.windowID);
              if(dataPtr != nullptr) {
                dataPtr->willUpdate = true;
              }
              break;
            }
            
          case SDL_WINDOWEVENT_SIZE_CHANGED:
          case SDL_WINDOWEVENT_DISPLAY_CHANGED:
            {
              auto dataPtr = getDataByWindowID(event.window.windowID);
              if(dataPtr != nullptr) {
                updateRenderer(dataPtr);
              }
              break;
            }
            
          case SDL_WINDOWEVENT_CLOSE:
            {
              auto dataPtr = getDataByWindowID(event.window.windowID);
              if(dataPtr != nullptr) {
                removeWindow(dataPtr->window);
              }
              break;
            }
          }
        }
        break;

      case SDL_JOYDEVICEADDED:
        addJoystick(event.jdevice.which);
        break;

      case SDL_JOYDEVICEREMOVED:
        removeJoystick(event.jdevice.which);
        break;
        
      default:
        if(event.type == jdiEventType) {
          // Animate events are automatically handled as part of checking for
          // updates.  No need to do anything.  In fact, just firing the event
          // is the whole point.
        }
        
        
      }
      
      if(_windowData.empty()) {
        _willExit = true;
      } else {
        if(event.type != jdiEventType) {
          // Do not propagate jdiEvents to the widgets.
          
          window_datum_type* focusDataPtr = getEventFocus(event);
          bool isHandled=false;
          
          if(focusDataPtr != nullptr) {
            isHandled = sendEvent(focusDataPtr, &event);
          } else {
            for(auto& data : _windowData) {
              if(isHandled) break;
              isHandled = sendEvent(&data, &event);
            }
          }
        }
        
        for(auto& data : _windowData) {
          resizeWidgets(&data);
          updateWidgets(&data);          
        }
      }
    }
  }
  
} // end namespace jdi
