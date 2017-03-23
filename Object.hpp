#include <string>
#include <vector>
#include <math.h>
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
        case 2:
          return 1.0;
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
    Coordinate getCenter();
    int getSize() const {return _coords.size();}
    void addCoordinate(double x, double y) {_coords.emplace_back(x,y);}
};

Coordinate Object::getCenter() {
  int size = _coords.size();
  double average_x, average_y;
  for (int i = 0; i < size; i++){
    average_x += _coords[i].getX();
    average_y += _coords[i].getY();
  }
  average_x /= size;
  average_y /= size;
  Coordinate c = Coordinate(average_x,average_y);
  return c;
}

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

enum RotationType { CENTER, ORIGIN, POINT };

template <int rows, int columns>
void translateMatrix(Coordinate vect, double translate_matrix[rows][columns]){
  for(int i = 0; i < rows; i++)
    for (int j= 0; j < columns; j++){
      if (i == j)
        translate_matrix[i][j] = 1;
      else
        translate_matrix[i][j] = 0;
    }
    for(int i = 0; i < columns; i++)
      translate_matrix[rows-1][i] = vect[i];
}

template <int rows, int columns>
void scaleMatrix(Coordinate factor, double scale_matrix[rows][columns]) {
  for(int i = 0; i < rows; i++)
    for (int j = 0; j < columns; j++){
      if(i == j)
        scale_matrix[i][j] = factor[j];
      else
        scale_matrix[i][j] = 0;
    }
}

template <int rows, int columns>
void rotateMatrix(int angle, double rotate_matrix[rows][columns]) {
  for(int i = 0; i < rows; i++)
    for(int j = 0; j<columns; j++)
      rotate_matrix[i][j] = 0;
  rotate_matrix[rows-1][columns-1] = 1;
  rotate_matrix[0][0] = cos(angle); rotate_matrix[1][1] = rotate_matrix[0][0];
  rotate_matrix[1][0] = sin(angle); rotate_matrix[0][1] = -rotate_matrix[1][0];
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

void scaleObject(Object* o, Coordinate factor) {
  Coordinate center = o->getCenter();
  
}

void rotateObject(Object* o,  double angle, RotationType rt, Coordinate reference) {

}

};
