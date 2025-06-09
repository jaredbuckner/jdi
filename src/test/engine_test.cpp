// File: t_engine.cpp
// ----
// Let's test out the engine in various ways.

#include <sstream>

#include "jdi.hpp"

class BlockWidget;
class ImageWidget;

typedef std::shared_ptr<BlockWidget> blockwidget_ptr;
typedef std::shared_ptr<ImageWidget> imagewidget_ptr;

typedef std::vector<blockwidget_ptr::weak_type> blockwidget_seq_type;

// A widget that is just a block of color
class BlockWidget : public jdi::Widget {
public:
  jdi::Color color;
  jdi::widget_ptr::weak_type linked_widget;  // Clicking me causes this linked widget to change visibility.
  Uint8 pct;  // From the left side, fill this percent of the block

  jdi::font_ptr   font;
  jdi::sprite_ptr text;
  
protected:
  BlockWidget();
  
public:
  virtual ~BlockWidget();

  virtual void onRenderUpdate(jdi::renderer_ptr renderer);
  virtual void onDraw(jdi::renderer_ptr renderer);
  virtual bool onEvent(jdi::renderer_ptr renderer,
                       SDL_Event* event);

  static blockwidget_ptr create();
}; // end class BlockWidget

BlockWidget::BlockWidget() : pct(100) {}
BlockWidget::~BlockWidget() {}

void BlockWidget::onRenderUpdate(jdi::renderer_ptr renderer) {
  if(text != nullptr) {
    text->generateTexture(renderer);
  }
}

void BlockWidget::onDraw(jdi::renderer_ptr renderer) {
  // Maybe Also SetRenderDrawBlendMode?
  jdi::safely(SDL_SetRenderDrawColor(renderer.get(),
                                     color.r,
                                     color.g,
                                     color.b,
                                     color.a), "Can't Color");
  const SDL_Rect* drawRect = getDrawRect();
  if(pct > 0) {
    if(pct >= 100) {
      SDL_RenderFillRect(renderer.get(),
                         drawRect);
    } else {
      SDL_Rect paintRect = *drawRect;
      paintRect.w *= pct;
      paintRect.w /= 100;
      SDL_RenderFillRect(renderer.get(),
                         &paintRect);
    }
  }

  if(text != nullptr) {
    std::ostringstream oss;    
    oss << "Jared was here:  " << int(pct) << "%";
    std::string foo(oss.str());
    text->strokeText(font, foo.c_str(), 0, jdi::Color::white());
    // text->strokeText(font, "");
    text->generateTexture(renderer);
    
    SDL_Point origin {drawRect->x, drawRect->y};
    text->drawFullClipped(renderer, &origin, drawRect);
  }
                          
  
}

bool BlockWidget::onEvent(jdi::renderer_ptr renderer,
                          SDL_Event* event) {
  switch(event->type) {
  case SDL_KEYUP:    
    if(event->key.keysym.sym == SDLK_ESCAPE) {
      jdi::engine_ptr engine = jdi::Engine::getEngine();
      
      engine->removeWindow(engine->getWindow(getSelf()));
      return(true);
    }
    break;

  case SDL_MOUSEBUTTONUP:
    {
      SDL_Point mouseLoc{event->button.x, event->button.y};
      
      if(isInside(&mouseLoc)) {
        jdi::widget_ptr widget = linked_widget.lock();
        
        if(widget) {
          jdi::engine_ptr engine = jdi::Engine::getEngine();
          
          widget->setVisible(!widget->isVisible());
          engine->requestResize(widget);
          engine->requestUpdate(widget);
        }
        
        return(true);
      }
    }
    break;
  }
  
  return(false);
}

blockwidget_ptr BlockWidget::create() {
  blockwidget_ptr reply(new BlockWidget());
  reply->setSelf(reply);

  return(reply);
}


// A widget that displays an image
class ImageWidget : public jdi::Widget {
public:
  std::string   fileName;

private:
  jdi::sprite_ptr sprite;

protected:
  ImageWidget() = default;

public:
  virtual ~ImageWidget() = default;
  virtual void onRenderUpdate(jdi::renderer_ptr renderer);
  virtual void onDraw(jdi::renderer_ptr renderer);

  void sizeToImage();

  static imagewidget_ptr create();
}; // end class ImageWidget

void ImageWidget::onRenderUpdate(jdi::renderer_ptr renderer) {
  if(sprite == nullptr && !fileName.empty()) {
    sprite = jdi::Sprite::createFromImage(fileName.c_str());
    sizeToImage();
  }
  sprite->generateTexture(renderer);
}

void ImageWidget::onDraw(jdi::renderer_ptr renderer) {
  if(sprite) {
    sprite->drawFull(renderer,
                     getDrawRect());
  }
}

void ImageWidget::sizeToImage() {
  SDL_Rect spriteBBox;
  if(sprite && sprite->getSrcBBox(&spriteBBox)) {
    setMinSize(spriteBBox.w,
               spriteBBox.h);
  }
}

imagewidget_ptr ImageWidget::create() {
  imagewidget_ptr reply(new ImageWidget());
  reply->setSelf(reply);
  
  return(reply);
}


struct dwc_data_type {
  int col;
  int row;
  int colSpan;
  int rowSpan;
  jdi::Color backColor;
  bool backAnimate;
  bool frontVisible;
  jdi::direction_type frontAnchors;
}; // end struct dwc_data_type


Uint32 animateBlocks(Uint32 interval, void* data) {
  blockwidget_seq_type *blocks = static_cast<blockwidget_seq_type*>(data);
  //Uint64 jiffies = SDL_GetTicks64() / 10;
  jdi::engine_ptr engine = jdi::Engine::getEngine();

  for(auto& weakBlock : *blocks) {
    blockwidget_ptr block = weakBlock.lock();

    if(block == nullptr) return(0);  // A block disappeared!  We must exit quickly!
    
    block->pct = (block->pct + 1) % 101;
    engine->requestUpdate(block);
  }
  return(interval);
}
  
  
extern "C" int main(int argc, char* argv[]) {
  dwc_data_type dwc_data[] = {
    {0, 0, 2, 1, jdi::Color::chocolate2(), 0, 1, jdi::JDI_NONE},
    {2, 0, 3, 1, jdi::Color::chocolate1(), 0, 1, jdi::JDI_E},
    {0, 1, 2, 1, jdi::Color::chameleon2(), 0, 1, jdi::JDI_W},
    {2, 1, 1, 2, jdi::Color::chameleon1(), 0, 0, jdi::JDI_EW},
    {3, 1, 2, 2, jdi::Color::chameleon0(), 0, 1, jdi::JDI_NSEW},
    {0, 2, 1, 3, jdi::Color::skyBlue2(),   0, 1, jdi::JDI_SEW},
    {1, 2, 1, 2, jdi::Color::skyBlue1(),   0, 1, jdi::JDI_NONE},
    {2, 3, 2, 1, jdi::Color::plum2(),      1, 0, jdi::JDI_NONE},
    {4, 3, 1, 2, jdi::Color::plum1(),      0, 1, jdi::JDI_NS},
    {1, 4, 2, 1, jdi::Color::scarlet2(),   0, 1, jdi::JDI_SEW},
    {3, 4, 1, 1, jdi::Color::scarlet1(),   0, 1, jdi::JDI_N}
  };
  
  SDL_Log("Starting the engine...");
  
  jdi::engine_ptr myEngine = jdi::Engine::getEngine();

  try {
    blockwidget_seq_type animatedBlocks;
    
    int maxDisplays = SDL_GetNumVideoDisplays();
    for(int displayID = 0; displayID < maxDisplays; ++displayID) {
      SDL_Rect dBounds;
      SDL_Rect duBounds;
      const char* dName = SDL_GetDisplayName(displayID);

      SDL_GetDisplayBounds(displayID, &dBounds);
      SDL_GetDisplayUsableBounds(displayID, &duBounds);

      SDL_Log(" D[%i] => %i+%i:%ix%i (%i+%i:%ix%i) %s",
              displayID,
              dBounds.x, dBounds.y, dBounds.w, dBounds.h,
              duBounds.x, duBounds.y, duBounds.w, duBounds.h,
              dName);
    }

    // Put these in a box so they don't live beyond the mainLoop!
    {
      jdi::window_ptr winOne = myEngine->createWindow("T_Engine");
      myEngine->setWindowBGColor(winOne, jdi::Color::scarlet0());

      jdi::grid_ptr gridOne = jdi::Grid::create();
      gridOne->setVisible(true);      
      gridOne->setColWeight(0, 1);
      gridOne->setColWeight(1, 1);
      gridOne->setColWeight(2, 1);
      gridOne->setColWeight(3, 1);
      gridOne->setRowWeight(0, 1);
      gridOne->setRowWeight(1, 1);
      gridOne->setRowWeight(2, 1);
      gridOne->setRowWeight(3, 1);
      gridOne->setAnchors(jdi::JDI_NSEW);

      for(int row = 0; row < 4; ++row) {
        for(int col = 0; col < 4; ++col) {
          blockwidget_ptr blockOne = BlockWidget::create();
          blockOne->setMinSize(32, 32);
          blockOne->setPadding(jdi::JDI_NSEW, 8);
          blockOne->setVisible(true);
          blockOne->color.set(0, 32 + 64 * row, 32 + 64 * col);
          blockOne->setAnchors(jdi::direction_type((col << 2) | row));
          gridOne->attachWidget(blockOne, row, col);
        }
      }

      // int count = 0;
      // for(jdi::widget_ptr iter = gridOne->getFirstPostOrderDFS();
      //     iter; iter = gridOne->getNextPostOrderDFS(iter)) {
      //   count += 1;
      //   SDL_Log("%i -> %p", count, iter.get());
      //   if(count > 100) break;
      // }
      myEngine->setRoot(winOne, gridOne);
    }

    {
      jdi::window_ptr winTwo = myEngine->createWindow("T_Engine2");
      myEngine->setWindowBGColor(winTwo, jdi::Color::aluminum0());

      jdi::grid_ptr gridTwo = jdi::Grid::create();
      gridTwo->setVisible(true);
      gridTwo->setColWeight(2, 2);
      gridTwo->setColWeight(3, 1);
      gridTwo->setRowWeight(1, 1);
      gridTwo->setRowWeight(2, 1);
      gridTwo->setPadding(jdi::JDI_NSEW, 8);      
      gridTwo->setAnchors(jdi::JDI_NSEW);
      
      for(unsigned int idx = 0; idx < (sizeof(dwc_data) / sizeof(dwc_data_type)); ++idx) {
        blockwidget_ptr blockTwo = BlockWidget::create();
        blockTwo->setMinSize(32, 32);
        blockTwo->setPadding(jdi::JDI_NSEW, 8);
        blockTwo->setVisible(true);
        blockTwo->color = dwc_data[idx].backColor;
        if(dwc_data[idx].backAnimate) {
          blockTwo->pct = 33;
          animatedBlocks.push_back(blockTwo);
          blockTwo->font = jdi::sdl_shared(TTF_OpenFontIndex("assets/FiraMono-Medium.ttf",
                                                             24, 0));
          blockTwo->text = jdi::sprite_ptr(new jdi::Sprite);                                           
        }        
        blockTwo->setAnchors(jdi::JDI_NSEW);
        gridTwo->attachWidget(blockTwo,
                              dwc_data[idx].row, dwc_data[idx].col,
                              dwc_data[idx].rowSpan, dwc_data[idx].colSpan);

        imagewidget_ptr imageTwo = ImageWidget::create();
        imageTwo->fileName = "assets/path1.png";        
        imageTwo->setAnchors(dwc_data[idx].frontAnchors);
        imageTwo->setVisible(dwc_data[idx].frontVisible);
        gridTwo->attachWidget(imageTwo,
                              dwc_data[idx].row, dwc_data[idx].col,
                              dwc_data[idx].rowSpan, dwc_data[idx].colSpan);

        blockTwo->linked_widget = imageTwo;
      }
      
      myEngine->setRoot(winTwo, gridTwo);      
    }    
    myEngine->setFrameRate(1000/60);  // 60fps
    myEngine->requestResizeAll();
    myEngine->requestUpdateAll();

    SDL_TimerID animateTimer = SDL_AddTimer(1000/100, animateBlocks, &animatedBlocks);  // 10 updates/sec
    
    myEngine->mainLoop();

    SDL_RemoveTimer(animateTimer);
  }
  catch(const jdi::Error& e) {
    if(e.where()[0]) {
      SDL_Log("SDL_Error: (%s) %s",
              e.where(), e.what());
    } else {
      SDL_Log("SDL_Error:  %s",
              e.what());
    }
  }
  catch(const std::exception& w) {
    SDL_Log("Unrecoverable error:  %s",
            w.what());
  }

  SDL_Log("Exiting...");

  return(0);
}
