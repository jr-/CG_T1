#include <cstring>
#include <vector>
#include "Coordinate.h"

class GObject {
public:
    DrawObject(){coordinates = vector<Coordinate>();}
    ~DrawObject(){}
    virtual Coordinate * getCoordinate() { return NULL; }
    virtual Coordinate * getCoordinate(unsigned int i) { return NULL; }
    
    string * name() { return _name; }
    virtual unsigned int size() { return _size; }
    
    bool operator==(GObject& go) { return _name == go->name()}
    
protected:
    string* _name;
    unsigned int _size;
    vector<Coordinate>* coordinates;
};

class Point : GObject{
public:
    Point(Coordinate c) {
        coordinates->push_back(c);
    }
    ~Point() {}
    Coordinate * getCoordinate() { return coords.front(); }
    
}

class Line : GObject {
public:
    Line(Coordinate c1, Coordinate c2) {
        coords = list<Coordinate>();        coords->push_back(c1);
        coords->push_back(c2);
    }
    ~Line() {}
    Coordinate * getCoordinate(unsigned int i) { if (i < size) return coords[i]; return NULL;}
}

class Poligon : GObject {
public:
    Poligon(vector<Coordinate>* coords) : _coords(coords) {}
    ~Poligon() {}
    Coordinate * getCoordinate(unsigned int i) { if (i < size) return coords[i]; return NULL;}
}
