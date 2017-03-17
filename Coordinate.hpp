//
//  Coordinate.h
//
//
//  Created by Andre de Carvalho on 16/03/17.
//
//

#ifndef Coordinate_h
#define Coordinate_h

class Coordinate {

public:
    Coordinate(double x, double y) : _x(x), _y(y) {}
    ~Coordinate() {}
    double x() { return _x; }
    double y() { return _y; }
    void move(double x, double y) { _x += x; _y += y; }
    Coordinate operator+=(const Coordinate& c) { _x += c._x; _y += c._y; return *this; }
    Coordinate operator-=(const Coordinate& c) { _x -= c._x; _y -= c._y; return *this; }
    Coordinate operator+(const Coordinate& c) { double a,b;  a = c._x + this->_x; b = c._y + this->_y; Coordinate coord(a,b);  return coord; }
    //Coordinate operator-(Coordinate& c) { return Coordinate(_x - c->x(), _y - c->y()); }

private:
    double _x, _y;
};

#endif /* Coordinate_h */
