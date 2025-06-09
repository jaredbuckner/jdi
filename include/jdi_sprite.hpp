// File: jdi_sprite.hpp
// ----
// Objects of this class represent a collection of renderable images.

namespace jdi {

  class Sprite {
  private:
    surface_ptr _surface;  // CPU-bound representation of the sprite
    texture_ptr _texture;  // GPU-bound representation of the sprite
    int _w;                 // Width of one element
    int _h;                 // Height of one element
    int _rows;              // Number of rows of elements in the surface
    int _cols;              // Number of cols of elements in the surface

  public:
    Sprite() = default;
    Sprite(const Sprite&) = delete;
    virtual ~Sprite();
    Sprite& operator=(const Sprite&) = delete;

    surface_ptr getSurface() const;
    surface_ptr claimSurface(SDL_Surface* sdl_surface,
                             int elementW=0, int elementH=0);
    surface_ptr strokeText(font_ptr font,
                           const char* text,
                           Uint32 wrapLength=0,
                           Color fg=Color::black(),
                           Color bg=Color::transparent());
    
    texture_ptr getTexture() const;
    texture_ptr generateTexture(renderer_ptr renderer);

    int getElementWidth() const;
    int getElementHeight() const;
    int getElementRows() const;
    int getElementCols() const;
    int getElementCount() const;

    int getElementIdx(int row, int col) const;
    int getWrappedElementIdx(int row, int col) const;

    bool getElementRowCol(int& row, int& col, int element) const;
    bool getWrappedElementRowCol(int& row, int& col, int element) const;
    
    // Returns true if element is in range and scroll does not clip out of
    // range.  Updates bboxRect, if non-null
    bool getSrcBBox(SDL_Rect* bboxRect,
                    int element=0,
                    SDL_Point* scrollPx=nullptr) const;
    
    // Select a portion of the element based on the selRect.
    // Returns true if the element is in range and the intersection of
    // selRect, first shifted into the element bbox, and the element bounding
    // box is not an empty box.  Updates bboxRect, if non-null
    bool selectSrcBBox(SDL_Rect* bboxRect,
                       const SDL_Rect* selRect,
                       int element=0,
                       SDL_Point* scrollPx=nullptr) const;
    
    // Copy an element onto the renderer in various ways.  Renderer and texture
    // must both be valid.  Returns true if the element was valid and the draw
    // was not entirely clipped.
    bool drawFull(renderer_ptr renderer,
                  const SDL_Point* tgtPoint,
                  int element=0,
                  SDL_Point* scrollPx=nullptr) const;
    bool drawFull(renderer_ptr renderer,
                  const SDL_Rect* tgtRect,
                  int element=0,
                  SDL_Point* scrollPx=nullptr) const;
    bool drawSelect(renderer_ptr renderer,
                    const SDL_Rect* selRect,
                    const SDL_Point* tgtPoint,
                    int element=0,
                    SDL_Point* scrollPx=nullptr) const;
    bool drawSelect(renderer_ptr renderer,
                    const SDL_Rect* selRect,
                    const SDL_Rect* tgtRect,
                    int element=0,
                    SDL_Point* scrollPx=nullptr) const;
    bool drawFullClipped(renderer_ptr renderer,
                         const SDL_Point* tgtPoint,
                         const SDL_Rect* clipRect,
                         int element=0,
                         SDL_Point* scrollPx=nullptr) const;
    bool drawFullClipped(renderer_ptr renderer,
                         const SDL_Rect* tgtRect,
                         const SDL_Rect* clipRect,
                         int element=0,
                         SDL_Point* scrollPx=nullptr) const;
    bool drawSelectClipped(renderer_ptr renderer,
                           const SDL_Rect* selRect,
                           const SDL_Point* tgtPoint,
                           const SDL_Rect* clipRect,
                           int element=0,
                           SDL_Point* scrollPx=nullptr) const;
    bool drawSelectClipped(renderer_ptr renderer,
                           const SDL_Rect* selRect,
                           const SDL_Rect* tgtRect,
                           const SDL_Rect* clipRect,
                           int element=0,
                           SDL_Point* scrollPx=nullptr) const;

    static bool createMaskRects(const SDL_Rect* srcRect,
                                const SDL_Rect* tgtRect,
                                const SDL_Rect* clipRect,
                                SDL_Rect* srcMaskRect,
                                SDL_Rect* tgtMaskRect);
    
    static sprite_ptr createEmpty(int elementW,
                                  int elementH,
                                  int rows,
                                  int cols);
    
    static sprite_ptr createFromImage(const char* file,
                                      int elementW = 0,
                                      int elementH = 0);

  }; // end class Sprite

  ////
  // Inline
  ////
  inline surface_ptr Sprite::getSurface() const { return(_surface); }
  
  inline texture_ptr Sprite::getTexture() const { return(_texture); }
  
  inline texture_ptr Sprite::generateTexture(renderer_ptr renderer) {
    _texture.reset();
    if(renderer && _surface) {
      _texture = sdl_shared(SDL_CreateTextureFromSurface(renderer.get(),
                                                         _surface.get()));
    }
    return(_texture);
  }

  inline int Sprite::getElementWidth() const {
    return(_surface == nullptr ? 0 : _w);
  }
  
  inline int Sprite::getElementHeight() const {
    return(_surface == nullptr ? 0 : _h);
  }
  
  inline int Sprite::getElementRows() const {
    return(_surface == nullptr ? 0 : _rows);
  }
  
  inline int Sprite::getElementCols() const {
    return(_surface == nullptr ? 0 : _cols);
  }
  
  inline int Sprite::getElementCount() const {
    return(_surface == nullptr ? 0 : _rows * _cols);
  }

  inline int Sprite::getElementIdx(int row, int col) const {
    return(_surface == nullptr ? 0
           : row * _cols + col);
  }
  
  inline int Sprite::getWrappedElementIdx(int row, int col) const {
    return(_surface == nullptr ? 0
           : (row % _rows) * _cols + (col % _cols));
  }
  
  inline bool Sprite::getElementRowCol(int& row, int& col,
                                       int element) const {
    if(_surface == nullptr) {
      return(false);
    } else {
      row = element / _cols;
      col = element % _cols;
      return(row < _rows);
    }
  }
  
  inline bool Sprite::getWrappedElementRowCol(int& row, int& col,
                                              int element) const {
    if(_surface == nullptr) {
      return(false);
    } else {
      row = (element / _cols) % _rows;
      col = (element % _cols);
      return(true);
    }
  }
  
    
    
} // end namespace jdi

    
