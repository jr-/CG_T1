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
      void set(double x, double y) { _x = x; _y = y; }
      double operator[](int i) {switch (i) {
        case 0:
          return _x;
        case 1:
          return _y;
        default:
          return 0.0;
      }}

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

namespace ObjectManipulation {

template <int rows, int columns>
void translateMatrix(Coordinate vect, double translate_matrix[rows][columns]){
  for(int i = 0; i < rows; i++)
    for (int j= 0; j < columns; j++){
      if (i == j)
        translate_matrix[i][j] = vect[j];
      else
        translate_matrix[i][j] = 0;
    }
}

template <int r1, int c1, int r2, int c2>
void matrix_multiplication(double a[r1][c1], double b[r2][c2], double result[r1][c1]) {
  for(int i = 0; i < r1; i++)
    for(int j=0; j< c1; j++)
      result[i][j] = 0.0;
  for(int i = 0; i < r1; i++)
    for(int j = 0; j < c2; j++)
      for (int k = 0; k<c1; k++)
        result[i][j] += a[i][k] * b[k][j];
}

void translateObject(Object* o, Coordinate vect) {
  vector<Coordinate> coords = o->getCoords();
  double a[1][2]; double b[2][2]; double result[1][2];
  translateMatrix<2,2>(vect, b);
  for (Coordinate c: coords) {
    a[0][0] = c.getX(); a[0][1] = c.getY();
    matrix_multiplication<1,2,2,2>(a,b,result);
    c.set(result[0][0], result[0][1]);
  }
}

void scaleObject(Object* o, double factor) {

}

};
