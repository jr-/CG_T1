//
//  Generic.cpp
//  
//
//  Created by Andre de Carvalho on 15/03/17.
//
//

#include <stdio.h>
#include <vector>
#include "Window.hpp"

class ViewPort {
public:
    ViewPort(unsigned int width, unsigned int height) : _w(width, height) {}
    ~ViewPort() {}
    
    Coordinate* position() { return _pos; }
    unsigned int width() { return _width; }
    unsigned int height() { return _height; }
    void zoom(double value) { _w.zoom(); }
    void moveTo (Coordinate * c) { w.moveTo(c); }
    void move(double x, double y) { w.move(x, y); }
private:
    unsigned int _width, _height
    Window _w;
    Coordinate * _pos;
public:
    
    Coordinate * transformCoordinate(Coordinate& c) {
        
    }
    
}

