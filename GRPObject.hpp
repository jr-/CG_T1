#include <string>
#include <vector>
#include "Coordinate.hpp"
using namespace std;
class GRPObject {
public:
    GRPObject(string name): _name(name) {_coords = vector<Coordinate>();}
    ~GRPObject(){}
    virtual Coordinate& getCoordinate();
    virtual Coordinate&  getCoordinate(unsigned int i);

    string name() { return _name; }
    virtual unsigned int size() { return _size; }

    bool operator==(GRPObject& go) { return _name == go.name();}

protected:
    string _name;
    unsigned int _size;
    vector<Coordinate> _coords;
};

class Point : GRPObject{
using GRPObject::_coords;
public:
    Point(string name, Coordinate c) : GRPObject(name){
        _coords.push_back(c); _size++;
    }
    ~Point() {}
    Coordinate& getCoordinate() { Coordinate c = _coords.front(); return c; }

};

class Line : GRPObject {
using GRPObject::_coords;
using GRPObject::_size;
public:
    Line(string name,Coordinate c1, Coordinate c2) : GRPObject(name) {
               _coords.push_back(c1); _size++;
        _coords.push_back(c2); _size++;
    }
    ~Line() {}
    Coordinate& getCoordinate(unsigned int i) { if (i < _size) {Coordinate c = _coords[i]; return c;} throw "Invalid index i";}
};

class Poligon : GRPObject {
using GRPObject::_coords;
using GRPObject::_size;
public:
    Poligon(string name, vector<Coordinate> coords) : GRPObject(name) { _coords = coords; }
    ~Poligon() {}
    Coordinate& getCoordinate(unsigned int i) { if (i < _size) return _coords[i]; throw "Invalid index i";}
};
