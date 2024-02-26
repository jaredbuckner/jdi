// File: jdi_engine.cpp
// ----
// Handle SDL init/mainloop/destroy; and other goodies.

#include "jdi.hpp"

namespace jdi {

  engine_ptr::weak_type& getSingletonEngine() {
    static engine_ptr::weak_type _hiddenEngine;

    return(_hiddenEngine);
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
  
  Engine::Engine() {
    if(getSingletonEngine().lock()) {
      throw(std::logic_error("Cannot have multiple simultaneous JDI Engines!"));
    }

    safely(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS));
    
  }

  Engine::~Engine() {
    _windowData.clear();  // Clear window data _before_ shutting down SDL
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

    renderer_ptr renderer
      = sdl_shared(SDL_CreateRenderer(window.get(),
                                      -1,   // First matching
                                      0));  // HW Accellerator requested but not required.
    
    
    _windowData.push_back(window_datum_type{window, renderer});

    _windowData.back().bbox.x = 0;
    _windowData.back().bbox.y = 0;
    _windowData.back().bgColor.set(255, 0, 255);

    safely(SDL_GetRendererOutputSize(_windowData.back().renderer.get(),
                                     &_windowData.back().bbox.w,
                                     &_windowData.back().bbox.h));
    
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

  const rgba_type& Engine::getWindowBGColor(window_ptr window) const {
    static rgba_type black;

    auto dataPtr = getDataByWindow(window);
    return(dataPtr == nullptr ? black : dataPtr->bgColor);
  }

  void Engine::setWindowBGColor(window_ptr window,
                                const rgba_type& color) {
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
        widget->onRenderUpdate(dataPtr->renderer);
        
        for(widget_ptr child = widget->getFirstDescendant();
            child; child = widget->getNextDescendant(child)) {
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
  
  void Engine::mainLoop() {
    _willExit = false;
    
    while(!_willExit) {
      SDL_Event event;
      if(1 != SDL_WaitEvent(&event)) throw(Error());
      
      switch(event.type) {
      case SDL_QUIT:
        _willExit = true;
        break;

      case SDL_KEYUP:
        if(event.key.keysym.sym == SDLK_F11) {
          auto dataPtr = getDataByWindowID(event.key.windowID);
          if(dataPtr != nullptr) {
            dataPtr->isBorderlessFS = !dataPtr->isBorderlessFS;
            if(dataPtr->isBorderlessFS) {
              SDL_SetWindowFullscreen(dataPtr->window.get(),
                                      SDL_WINDOW_FULLSCREEN_DESKTOP);
            } else {
              SDL_SetWindowFullscreen(dataPtr->window.get(), 0);      
            }
          }
        } else {
          // Pass the event up the chain, will ya?
        }
          
        
      case SDL_WINDOWEVENT:
        //// BEGIN WINDOW EVENT SWITCH
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
                dataPtr->renderer.reset();  // Destroy the old renderer            
                dataPtr->renderer
                  = sdl_shared(SDL_CreateRenderer(dataPtr->window.get(),
                                                  -1,   // First matching
                                                  0));  // HW Accellerator requested but not required.
                
                safely(SDL_GetRendererOutputSize(dataPtr->renderer.get(),
                                                 &(dataPtr->bbox.w),
                                                 &(dataPtr->bbox.h)));
                
                if(dataPtr->root) {
                  dataPtr->root->onRenderUpdate(dataPtr->renderer);
                  
                  for(widget_ptr iter = dataPtr->root->getFirstDescendant();
                      iter; iter = dataPtr->root->getNextDescendant(iter)) {
                    iter->onRenderUpdate(dataPtr->renderer);
                  }
                }
                
                dataPtr->willResize = true;
                dataPtr->willUpdate = true;            
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
        //// END WINDOW EVENT SWITCH
      }
      
      if(_windowData.empty()) {
        _willExit = true;
      } else {
        for(auto& data : _windowData) {
          if(data.willResize && data.root && data.root->isVisible()) {
            data.root->setDrawRect(&data.bbox);
            data.root->onResize(data.renderer);
          }
          data.willResize = false;
          if(data.willUpdate) {
            SDL_SetRenderDrawColor(data.renderer.get(),
                                   data.bgColor.r,
                                   data.bgColor.g,
                                   data.bgColor.b,
                                   data.bgColor.a);
            safely(SDL_RenderClear(data.renderer.get()));            
            if(data.root && data.root->isVisible()) {              
              data.root->onDraw(data.renderer);
            }
            SDL_RenderPresent(data.renderer.get());
          }
          data.willUpdate = false;
        }
      }
    }
  }
  
} // end namespace jdi
