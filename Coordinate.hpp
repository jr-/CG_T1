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
    Coordinate(int x, int y) : _x(x), _y(y) {}
    ~Coordinate() {}
    double x() { return _x; }
    double y() { return _y; }
    void move(double x, double y) { _x += x; _y += y; }
    Coordinate& operator+=(Coordinate& c) { _x += c.x(); _y += c.y(); return this; }
    Coordinate& operator-=(Coordinate& c) { _x -= c.x(); _y -= c.y(); return this; }
    Coordinate& operator+(Coordinate& c) { return Coordinate(_x + c.x(), _y + c.y()); }
    Coordinate& operator-(Coordinate& c) { return Coordinate(_x - c.x(), _y - c.y()); }
    
private:
    double _x, _y;
}

#endif /* Coordinate_h */
