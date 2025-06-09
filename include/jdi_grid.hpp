// File: grid.hpp
// ----
// A grid is a widget that houses other widgets

#include <deque>

namespace jdi {

  class Grid : public Widget {
    struct child_data {
      widget_ptr widget;
      SDL_Rect loc;
    };
    
    typedef std::deque<child_data>  child_container_type;
    typedef std::deque<int>         weight_container_type;
    
    child_container_type  _children;
    weight_container_type _rowWeight;
    weight_container_type _colWeight;

    void totalElemAndWeight(const weight_container_type& elements,
                            const weight_container_type& elemWeights,
                            int idx, int stop,
                            int& totalElem, int& totalWeight) const;
    void expandElements(weight_container_type& elements,
                        const weight_container_type& elemWeights,
                        int idx, int stop,
                        int expand, int totalWeight) const;

  protected:
    Grid() = default;

  public:
    virtual ~Grid();
    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;

    virtual void onDraw(renderer_ptr renderer);
    virtual void onResize(renderer_ptr renderer);
    
    virtual bool hasChildren() const;
    virtual bool hasChild(widget_ptr child) const;
    virtual widget_ptr getFirstChild(widget_ptr prune=nullptr) const;
    virtual widget_ptr getNextChild(widget_ptr child,
                                    widget_ptr prune=nullptr) const;
    
    bool attachWidget(widget_ptr child,
                      int row=0, int col=0,
                      int rowSpan=1, int colSpan=1);
    
    void setColWeight(int x, int weight);
    void setRowWeight(int y, int weight);
    
    static grid_ptr create();
    
  }; // end class Grid

  
  
} // end namespace jdi
