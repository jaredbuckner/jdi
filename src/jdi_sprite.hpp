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

    int getElementCount() const;

    // Returns true if element is in range.  Updates bboxRect, if non-null
    bool getSrcBBox(SDL_Rect* bboxRect,
                    int element=0) const;

    // Select a portion of the element based on the selRect.
    // Returns true if the element is in range and the intersection of
    // selRect, first shifted into the element bbox, and the element bounding
    // box is not an empty box.  Updates bboxRect, if non-null
    bool selectSrcBBox(SDL_Rect* bboxRect,
                       const SDL_Rect* selRect,
                       int element=0) const;
    
    // Copy an element onto the renderer in various ways.  Renderer and texture
    // must both be valid.  Returns true if the element was valid and the draw
    // was not entirely clipped.
    bool drawFull(renderer_ptr renderer,
                  const SDL_Point* tgtPoint,
                  int element=0) const;
    bool drawFull(renderer_ptr renderer,
                  const SDL_Rect* tgtRect,
                  int element=0) const;
    bool drawSelect(renderer_ptr renderer,
                    const SDL_Rect* selRect,
                    const SDL_Point* tgtPoint,
                    int element=0) const;
    bool drawSelect(renderer_ptr renderer,
                    const SDL_Rect* selRect,
                    const SDL_Rect* tgtRect,
                    int element=0) const;
    bool drawFullClipped(renderer_ptr renderer,
                         const SDL_Point* tgtPoint,
                         const SDL_Rect* clipRect,
                         int element=0) const;
    bool drawFullClipped(renderer_ptr renderer,
                         const SDL_Rect* tgtRect,
                         const SDL_Rect* clipRect,
                         int element=0) const;
    bool drawSelectClipped(renderer_ptr renderer,
                           const SDL_Rect* selRect,
                           const SDL_Point* tgtPoint,
                           const SDL_Rect* clipRect,
                           int element=0) const;
    bool drawSelectClipped(renderer_ptr renderer,
                           const SDL_Rect* selRect,
                           const SDL_Rect* tgtRect,
                           const SDL_Rect* clipRect,
                           int element=0) const;

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

  inline int Sprite::getElementCount() const {
    return(_surface == nullptr ? 0 : _rows * _cols);
  }
  
} // end namespace jdi

    
