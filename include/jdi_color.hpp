// File: jdi_color.hpp
// ----
// Declarations for RGBA colors

namespace jdi {

  class Color {
  public:
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    Color();
    Color(Uint8 red,
          Uint8 green,
          Uint8 blue,
          Uint8 alpha=255);
    Color(const Color&) = default;
    Color& operator=(const Color&) = default;
    ~Color() = default;
    
    Color& set(Uint8 red,
               Uint8 green,
               Uint8 blue,
               Uint8 alpha=255);
    
    void fillFromPixel(Uint32 pixel,
                       const SDL_PixelFormat* format);
    Uint32 mapToPixel(const SDL_PixelFormat* format) const;

    SDL_Color getSDL() const;
    void setSDL(SDL_Color color);
    
    static Color black();
    static Color white();
    static Color transparent();
    static Color butter0();
    static Color butter1();
    static Color butter2();
    static Color orange0();
    static Color orange1();
    static Color orange2();
    static Color chocolate0();
    static Color chocolate1();
    static Color chocolate2();
    static Color chameleon0();
    static Color chameleon1();
    static Color chameleon2();
    static Color skyBlue0();
    static Color skyBlue1();
    static Color skyBlue2();
    static Color plum0();
    static Color plum1();
    static Color plum2();
    static Color scarlet0();
    static Color scarlet1();
    static Color scarlet2();
    static Color aluminum0();
    static Color aluminum1();
    static Color aluminum2();
    static Color aluminum3();
    static Color aluminum4();
    static Color aluminum5();

    static Uint8 alphaInterp(Uint8 a, Uint8 alpha,
                             Uint8 b, Uint8 beta);
    static Uint8 scaledMul(Uint8 a, Uint8 b);

  }; // end class Color


  inline Uint8 Color::alphaInterp(Uint8 a, Uint8 alpha,
                                  Uint8 b, Uint8 beta) {
    Uint16 aa = Uint16(a) * Uint16(alpha);
    Uint16 bb = Uint16(b) * Uint16(beta);
    Uint16 cc = (aa + bb) / 255;
    return(cc);
  }

  inline Uint8 Color::scaledMul(Uint8 a, Uint8 b) {
    Uint16 cc = Uint16(a) * Uint16(b) / 255;
    return(cc);
  }

  // Comparisons
  inline bool operator==(const Color& a,
                         const Color& b) {
    return(a.r == b.r &&
           a.g == b.g &&
           a.b == b.b &&
           a.a == b.a);
  }

  inline bool operator!=(const Color& a, const Color& b) { return(!(a==b)); }
  
  inline bool operator<(const Color& a,
                        const Color& b) {
    return(a.r < b.r ||
           (! (b.r < a.r) &&
            (a.g < b.g ||
             (! (b.g < a.g) &&
              (a.b < b.b ||
               (! (b.b < a.b) &&
                a.a < b.a))))));
  }

  inline bool operator>(const Color& a, const Color& b) { return(b < a); }
  inline bool operator<=(const Color& a, const Color& b) { return(!(b < a)); }
  inline bool operator>=(const Color& a, const Color& b) { return(!(a < b)); }

  ////
  // Operations
  ////
  
  // Compose  (b over a)
  inline Color& operator+=(Color& a, const Color& b) {
    Uint8 alpha = b.a;
    Uint8 beta  = 255 - alpha;
    
    a.r = Color::alphaInterp(b.r, alpha, a.r, beta);
    a.g = Color::alphaInterp(b.g, alpha, a.g, beta);
    a.b = Color::alphaInterp(b.b, alpha, a.b, beta);
    a.a = Color::alphaInterp(255, alpha, a.a, beta);
    
    return(a);
  }

  inline Color operator+(const Color& a, const Color& b) {
    Color reply = a;
    reply += b;
    return(reply);
  }

  // Multiply
  inline Color& operator*=(Color& a, const Color& b) {
    a.r = Color::scaledMul(a.r, b.r);
    a.g = Color::scaledMul(a.g, b.g);
    a.b = Color::scaledMul(a.b, b.b);
    a.a = Color::scaledMul(a.a, b.a);
    
    return(a);
  }

  inline Color operator*(const Color& a, const Color& b) {
    Color reply = a;
    reply *= b;
    return(reply);
  }

  // Screen
  inline Color& operator/=(Color& a, const Color& b) {
    a.r = 255 - Color::scaledMul(255 - a.r, 255 - b.r);
    a.g = 255 - Color::scaledMul(255 - a.g, 255 - b.g);
    a.b = 255 - Color::scaledMul(255 - a.b, 255 - b.b);
    a.a = 255 - Color::scaledMul(255 - a.a, 255 - b.a);

    return(a);
  }

  inline Color operator/(const Color& a, const Color& b) {
    Color reply = a;
    reply /= b;
    return(reply);
  }
  
  inline Color::Color() : r(0), g(0), b(0), a(255) {}

  inline Color::Color(Uint8 red,
                      Uint8 green,
                      Uint8 blue,
                      Uint8 alpha) :
    r(red),
    g(green),
    b(blue),
    a(alpha) {}

  inline Color& Color::set(Uint8 red,
                           Uint8 green,
                           Uint8 blue,
                           Uint8 alpha) {
    r = red;
    g = green;
    b = blue;
    a = alpha;
    return(*this);
  }

  inline void Color::fillFromPixel(Uint32 pixel,
                                   const SDL_PixelFormat* format) {
    SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
  }
  
  inline Uint32 Color::mapToPixel(const SDL_PixelFormat* format) const {
    return(SDL_MapRGBA(format, r, g, b ,a));
  }
  
  inline SDL_Color Color::getSDL() const {
    SDL_Color reply = SDL_Color{r, g, b, a};
    return(reply);
  }

  inline void Color::setSDL(SDL_Color color) {
    r = color.r; g = color.g; b = color.b, a = color.a;
  }
  
  inline Color Color::black()       { return(Color(0x00, 0x00, 0x00)); }
  inline Color Color::white()       { return(Color(0xff, 0xff, 0xff)); }
  inline Color Color::transparent() { return(Color(0x00, 0x00, 0x00, 0x00)); }
  inline Color Color::butter0()     { return(Color(0xc4, 0xa0, 0x00)); }
  inline Color Color::butter1()     { return(Color(0xed, 0xd4, 0x00)); }
  inline Color Color::butter2()     { return(Color(0xfc, 0xe9, 0x4f)); }
  inline Color Color::orange0()     { return(Color(0xce, 0x5c, 0x00)); }
  inline Color Color::orange1()     { return(Color(0xf5, 0x79, 0x00)); }
  inline Color Color::orange2()     { return(Color(0xfc, 0xaf, 0x3e)); }
  inline Color Color::chocolate0()  { return(Color(0x8f, 0x59, 0x02)); }
  inline Color Color::chocolate1()  { return(Color(0xc1, 0x7d, 0x11)); }
  inline Color Color::chocolate2()  { return(Color(0xe9, 0xb9, 0x6e)); }
  inline Color Color::chameleon0()  { return(Color(0x4e, 0x9a, 0x06)); }
  inline Color Color::chameleon1()  { return(Color(0x73, 0xd2, 0x16)); }
  inline Color Color::chameleon2()  { return(Color(0x8a, 0xe2, 0x34)); }
  inline Color Color::skyBlue0()    { return(Color(0x20, 0x4a, 0x87)); }
  inline Color Color::skyBlue1()    { return(Color(0x34, 0x65, 0xa4)); }
  inline Color Color::skyBlue2()    { return(Color(0x72, 0x9f, 0xcf)); }
  inline Color Color::plum0()       { return(Color(0x5c, 0x35, 0x66)); }
  inline Color Color::plum1()       { return(Color(0x75, 0x50, 0x7b)); }
  inline Color Color::plum2()       { return(Color(0xad, 0x7f, 0xa8)); }
  inline Color Color::scarlet0()    { return(Color(0xa4, 0x00, 0x00)); }
  inline Color Color::scarlet1()    { return(Color(0xcc, 0x00, 0x00)); }
  inline Color Color::scarlet2()    { return(Color(0xef, 0x29, 0x29)); }
  inline Color Color::aluminum0()   { return(Color(0x2e, 0x34, 0x36)); }
  inline Color Color::aluminum1()   { return(Color(0x55, 0x57, 0x53)); }
  inline Color Color::aluminum2()   { return(Color(0x88, 0x8a, 0x85)); }
  inline Color Color::aluminum3()   { return(Color(0xba, 0xbd, 0xb6)); }
  inline Color Color::aluminum4()   { return(Color(0xd3, 0xd7, 0xcf)); }
  inline Color Color::aluminum5()   { return(Color(0xee, 0xee, 0xec)); }
  
} // end namespace jdi
