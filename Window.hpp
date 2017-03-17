//
//  Generic.cpp
//  
//
//  Created by Andre de Carvalho on 15/03/17.
//
//

#include <stdio.h>
#include <vector>
#include "GRPObject.hpp"

class Window {
public:
    Window(unsigned int width, unsigned int height) : _width(width), _height(height) {}
    ~Window() {}
    
    Coordinate* position() { return _pos; }
    unsigned int width() { return _width; }
    unsigned int height() { return _height; }
    void zoom(double value) {
        _width -= value;
        _height -= value;
        value = value / 2;
        _pos.move(value, value);
    }
    
    void moveTo(Coordinate * c) { _pos = c; }
    void move(double x, double y) { _pos.move(x, y); }
    
private:
    unsigned int _width, _height;
    Coordinate * _pos;
    
}

