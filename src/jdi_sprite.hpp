// File: jdi_sprite.hpp
// ----
// Objects of this class represent a collection of renderable images.

namespace jdi {

  class Sprite {
  private:
    surface_ptr _surface;  // CPU-bound representation of the sprite
    texture_ptr _texture;  // GPU-bound representation of the sprite
    int w;                 // Width of one element
    int h;                 // Height of one element
    int rows;              // Number of rows of elements in the surface
    int cols;              // Number of cols of elements in the surface

  public:
    Sprite() = default;
    Sprite(const Sprite&) = delete;
    virtual ~Sprite();
    Sprite& operator=(const Sprite&) = delete;

    surface_ptr getSurface() const;
    surface_ptr claimSurface(SDL_Surface* sdl_surface,
                             int elementW=0, int elementH=0);
    
    texture_ptr getTexture() const;
    texture_ptr generateTexture(renderer_ptr renderer);

    int getElementCount() const;

    // Returns true if element is in range.  Updates bboxRect, if non-null
    bool getBBox(SDL_Rect* bboxRect,
                 int element=0) const;

    // Copy the sprite element onto the renderer
    void renderCopy(renderer_ptr renderer,
                    const SDL_Rect* tgtRect,
                    int element=0) const;

    // Copy the sprite element onto the renderer, clipping it as needed.
    void clippedRenderCopy(renderer_ptr renderer,
                           const SDL_Rect* tgtRect,
                           const SDL_Rect* clipRect,
                           int element=0) const;


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
    return(_surface == nullptr ? 0 : rows * cols);
  }

  inline void Sprite::renderCopy(renderer_ptr renderer,
                                 const SDL_Rect* tgtRect,
                                 int element) const {
    SDL_Rect srcRect;
    if(getBBox(&srcRect, element)) {
      SDL_RenderCopy(renderer.get(),
                     _texture.get(),
                     &srcRect,
                     tgtRect);
    }
  }

  inline void Sprite::clippedRenderCopy(renderer_ptr renderer,
                                        const SDL_Rect* tgtRect,
                                        const SDL_Rect* clipRect,
                                        int element) const {
    SDL_Rect srcRect;
    if(getBBox(&srcRect, element)) {
      clipped_render_copy(renderer,
                          _texture,
                          &srcRect,
                          tgtRect,
                          clipRect);
    }
  }
  
} // end namespace jdi

    
