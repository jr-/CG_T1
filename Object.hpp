#include <string>
#include <vector>
using namespace std;

class Coordinate {
  public:
      Coordinate(double x, double y) : _x(x), _y(y) {
      }
      double getX() const { return _x;}
      double getY() const { return _y;}
      void move(double x, double y) { _x += x; _y += y; }

  private:
      double _x, _y;
};

enum ObjectType{POINT, LINE, POLYGON};

class Object {
  private:
       string _name;
       vector<Coordinate> _coords;
       ObjectType _type;
  public:
    Object(string name, ObjectType type) : _name(name), _type(type) {}
    virtual ~Object() {}
    string getName() const { return _name;}
    ObjectType getType() const {return _type;}
    vector<Coordinate>& getCoords() {return _coords;}
    int getSize() const {return _coords.size();}
    void addCoordinate(double x, double y) {_coords.emplace_back(x,y);}
};
//TODO usar enum??
class Point: public Object {
  public:
    Point(string name) : Object(name, ObjectType::POINT) {}
};

class Line: public Object {
  public:
    Line(string name) : Object(name, ObjectType::LINE) {}
};

class Polygon: public Object {
  public:
    Polygon(string name) : Object(name, ObjectType::POLYGON) {}
};
