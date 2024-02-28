// File: jdi_grid.cpp
// ----
// Grid class implementation.  The good stuff.

#include <algorithm>

#include "jdi.hpp"

namespace jdi {

  void Grid::totalElemAndWeight(const Grid::weight_container_type& elements,
                                const Grid::weight_container_type& elemWeights,
                                int idx, int stop,
                                int& totalElem,
                                int& totalWeight) const {
    totalElem = 0;
    totalWeight = 0;
    
    for(;idx < stop; ++idx) {
      totalElem += elements[idx];
      totalWeight += elemWeights[idx];
    }
  }  

  void Grid::expandElements(Grid::weight_container_type& elements,
                            const Grid::weight_container_type& elemWeights,
                            int idx, int stop,
                            int expand, int totalWeight) const {
    int each = expand / totalWeight;
    int remainder = expand % totalWeight;
    
    for(;idx < stop; ++idx) {
      int partial = std::min(elemWeights[idx], remainder);
      elements[idx] += each * elemWeights[idx] + partial;
      remainder -= partial;
    }    
  }
                            
  Grid::~Grid() {}

  void Grid::onDraw(renderer_ptr renderer) {
    for(auto& childData : _children) {
      childData.widget->onDraw(renderer);
    }
  }

  void Grid::onResize(renderer_ptr renderer) {
    weight_container_type rowHeights(_rowWeight.size());
    weight_container_type colWidths(_colWeight.size());
    weight_container_type rowUnits(_rowWeight.size(), 1);
    weight_container_type colUnits(_colWeight.size(), 1);
    
    const SDL_Rect* drawRect = getDrawRect();
    
    // Go once through the children, setting widths and heights based on
    // minsize+padding of each single-span widget
    for(auto& childData : _children) {
      if(childData.loc.w == 1) {
        int minW = childData.widget->getMinW() + childData.widget->getPadding(JDI_EW);
        int colW = colWidths[childData.loc.x];
        colWidths[childData.loc.x] = std::max(minW, colW);
      }

      if(childData.loc.h == 1) {
        int minH = childData.widget->getMinH() + childData.widget->getPadding(JDI_NS);
        int rowH = rowHeights[childData.loc.y];
        rowHeights[childData.loc.y] = std::max(minH, rowH);
      }
    }
    
    // Go one through the children, expanding widths and heights based on
    // minsize+padding of each multi-span widget, apportioning space using
    // weights.
    for(auto& childData : _children) {
      // Widths
      if(childData.loc.w != 1) {
        int minW = childData.widget->getMinW() + childData.widget->getPadding(JDI_EW);
        int colW = 0;
        int totalWeight = 0;

        totalElemAndWeight(colWidths, _colWeight,
                           childData.loc.x, childData.loc.x + childData.loc.w,
                           colW, totalWeight);
        
        // Do we expand?
        if(minW > colW) {
          
          // Expand by # columns if no weight
          if(totalWeight == 0) {
            expandElements(colWidths, colUnits,
                           childData.loc.x, childData.loc.x + childData.loc.w,
                           minW - colW, childData.loc.w);
          } else {
            expandElements(colWidths, _colWeight,
                           childData.loc.x, childData.loc.x + childData.loc.w,
                           minW - colW, totalWeight);
          }
        }
      }
      
      if(childData.loc.h != 1) {
        // Heights
        int minH = childData.widget->getMinH() + childData.widget->getPadding(JDI_NS);
        int rowH = 0;
        int totalWeight = 0;
        
        totalElemAndWeight(rowHeights, _rowWeight,
                           childData.loc.y, childData.loc.y + childData.loc.h,
                           rowH, totalWeight);
        
        if(minH > rowH) {
          // Expand by # rows if no weight          
          if(totalWeight == 0) {
            expandElements(rowHeights, rowUnits,
                           childData.loc.y, childData.loc.y + childData.loc.h,
                           minH - rowH, childData.loc.h);
          } else {
            expandElements(rowHeights, _rowWeight,
                           childData.loc.y, childData.loc.y + childData.loc.h,
                           minH - rowH, totalWeight);
          }
        }
      }
    }
    
    // Now, check the full size and distribute any remaining row/column
    // according to the total weights.  This time, do not expand if there is no
    // weight for expansion.
    {
      // Widths
      int minW = 0;
      int totalWeight = 0;
      
      totalElemAndWeight(colWidths, _colWeight,
                         0, colWidths.size(),
                         minW, totalWeight);
      
      if(totalWeight > 0 && drawRect->w > minW) {
        expandElements(colWidths, _colWeight,
                       0, colWidths.size(),
                       drawRect->w - minW, totalWeight);
      }
    }
    
    {
      // Heights
      int minH = 0;
      int totalWeight = 0;
      
      totalElemAndWeight(rowHeights, _rowWeight,
                         0, rowHeights.size(),
                         minH, totalWeight);
      
      if(totalWeight > 0 && drawRect->h > minH) {
        expandElements(rowHeights, _rowWeight,
                       0, rowHeights.size(),
                       drawRect->h - minH, totalWeight);
      }
    }
    
    // Now we have all the data!  Let's size!
    for(auto& childData : _children) {
      SDL_Rect newBound = {0,0,0,0};
      int stopX = childData.loc.x + childData.loc.w;
      int stopY = childData.loc.y + childData.loc.h;
      
      for(int idx = 0; idx < stopX; ++idx) {
        int colW = colWidths[idx];
        if(idx < childData.loc.x) {
          newBound.x += colW;
        } else {
          newBound.w += colW;
        }
      }
      
      for(int idx = 0; idx < stopY; ++idx) {
        int rowH = rowHeights[idx];
        if(idx < childData.loc.y) {
          newBound.y += rowH;
        } else {
          newBound.h += rowH;
        }
      }

      childData.widget->setDrawRect(&newBound);
      childData.widget->onResize(renderer);
    }
  }

  bool Grid::hasChildren() const { return(!_children.empty()); }

  bool Grid::hasChild(widget_ptr child) const {
    for(auto& childData : _children) {
      if(childData.widget == child) { return(true); }
    }
    return(false);
  }

  widget_ptr Grid::getFirstChild(widget_ptr prune) const {
    if(_children.empty())            { return(nullptr); }
    if(_children[0].widget != prune) { return(_children[0].widget); }
    if(_children.size() == 1)        { return(nullptr); }
    else                             { return(_children[1].widget); }
  }

  widget_ptr Grid::getNextChild(widget_ptr child,
                                widget_ptr prune) const {
    if(child == nullptr) { return(getFirstChild(prune)); }
    
    for(auto iter = _children.begin();
        iter != _children.end(); ++iter) {
      if(iter->widget == child) {
        ++iter;
        if(iter == _children.end()) { return(nullptr); }
        if(iter->widget != prune)   { return(iter->widget); }
        ++iter;
        if(iter == _children.end()) { return(nullptr); }
                                      return(iter->widget);
      }
    }
    
    return(nullptr);
  }

  bool Grid::attachWidget(widget_ptr child,
                          int row, int col,
                          int rowSpan, int colSpan) {
    if(!claimChild(child)) { return(false); }
    
    _children.push_back(child_data{child});
    child_data* dataPtr = &(_children.back());
    dataPtr->loc.x = std::max(0, col);
    dataPtr->loc.y = std::max(0, row);
    dataPtr->loc.w = std::max(1, colSpan);
    dataPtr->loc.h = std::max(1, rowSpan);

    if(unsigned(dataPtr->loc.w) >= _colWeight.size()) {
      _colWeight.resize(dataPtr->loc.w+1);
    }

    if(unsigned(dataPtr->loc.h) >= _rowWeight.size()) {
      _rowWeight.resize(dataPtr->loc.h+1);
    }
    
    return(true);
  }

  void Grid::setColWeight(int x, int weight) {
    if(x >= 0) {
      if(unsigned(x) >= _colWeight.size()) {
        _colWeight.resize(x+1);
      }
      _colWeight[x] = std::max(0, weight);      
    }
  }

  void Grid::setRowWeight(int y, int weight) {
    if(y >= 0) {
      if(unsigned(y) >= _rowWeight.size()) {
        _rowWeight.resize(y+1);
      }
      _rowWeight[y] = std::max(0, weight);      
    }
  }
  
  grid_ptr Grid::create() {
    grid_ptr reply(new Grid());
    reply->setSelf(reply);

    return(reply);    
  }
  
} // end namespace jdi
