// File: jdi_text.hpp
// ----
// Objects of this class represent a renderable region of text.

namespace jdi {

  class Text {
  private:
    surface_ptr _surface;
    texture_ptr _texture;

    // We will share ownership of the font.  Last one to use it can turn the
    // lights out...
    font_ptr _font;
    int _wrap;  // If positive, render text wrapped at the given number of pixels
    Color _fgColor;
    Color _bgColor;

    std::string _str;

  public:
    Text();
    Text(const Text&) = delete;
    virtual ~Text();
    Text& operator=(const Text&) = delete;

    surface_ptr getSurface() const;
    surface_ptr drawText();  // Render current text with current settings into
                             // surface.

    texture_ptr getTexture() const;
    texture_ptr generateTexture(renderer_ptr renderer);

    font_ptr getFont() const;
    void setFont(font_ptr font);

    int getWrap() const;
    void setWrap(int wrap);
    
    Color getFGColor() const;
    void setFGColor(Color color);

    Color getBGColor() const;
    void setBGColor(Color color);

    const std::string& getString() const;
    std::string& getString();
    
    bool getBBox(SDL_Rect* bboxRect);
    void renderCopy(renderer_ptr renderer,
                    const SDL_Rect* tgtRect) const;

  }; // end class Text
  
  ////
  // Inline
  ////
  inline Text::Text() :
    _surface(),
    _texture(),
    _font(),
    _wrap(0),
    _fgColor(Color::black()),
    _bgColor(Color::transparent()),
    _str() {}
  
  inline surface_ptr Text::getSurface() const { return(_surface); }
  
  inline texture_ptr Text::getTexture() const { return(_texture); }
  
  inline texture_ptr Text::generateTexture(renderer_ptr renderer) {
    _texture.reset();
    if(renderer && _surface) {
      _texture = sdl_shared(SDL_CreateTextureFromSurface(renderer.get(),
                                                         _surface.get()));
    }
    return(_texture);
  }
  
  inline font_ptr Text::getFont() const { return(_font); }
  inline void Text::setFont(font_ptr font) { _font = font; }
  
  inline int Text::getWrap() const { return(_wrap); }
  inline void Text::setWrap(int wrap) { _wrap = wrap; }
  
  inline Color Text::getFGColor() const { return(_fgColor); }
  inline void Text::setFGColor(Color color) { _fgColor = color; };
  inline Color Text::getBGColor() const { return(_bgColor); }
  inline void Text::setBGColor(Color color) { _bgColor = color; };
  
  inline const std::string& Text::getString() const { return(_str); }
  inline std::string& Text::getString() { return(_str); }
  
  inline void Text::renderCopy(renderer_ptr renderer,
                               const SDL_Rect* tgtRect) const {
    if(_texture != nullptr) {
      SDL_RenderCopy(renderer.get(),
                     _texture.get(),
                     nullptr,
                     tgtRect);
    }
  }
  
} // end namespace jdi
