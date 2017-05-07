#include <string>
#include <vector>
#include <math.h>
#include "Matrix.hpp"
#include "Coordinate.hpp"
#include <iostream>
using namespace std;
extern const double PI;

enum ObjectType { POINT, LINE, POLYGON, WINDOW };
class Window;
class Object {
protected:
  vector<Coordinate> _coords;
  vector<Coordinate> _ncoords;
  Coordinate getCenter();
private:
   string _name;
   ObjectType _type;
public:
  enum RotationType { CENTER, ORIGIN, POINT };
  Object(string name, ObjectType type) : _name(name), _type(type) {}
  virtual ~Object() {}
  string getName();
  ObjectType getType() const;
  vector<Coordinate>& getCoords();
  vector<Coordinate>& getNCoords();
  int getSize() const;
  void addCoordinate(double x, double y);
  void addNCoordinate(double x, double y);
  void addNCoordinate(Coordinate c);
  void updateNCoordinate(Matrix scn_matrix);
  void translate(Coordinate vect);
  void scale(Coordinate factor);
  void print();
  virtual void rotate(double angle, RotationType rt, Coordinate reference=Coordinate(0.0,0.0));
};

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

namespace ObjectManipulationMatrix {

template <int rows, int columns>
void translate_matrix(Coordinate vect, Matrix& translate_matrix){
  for(int i = 0; i < rows; i++)
    for (int j= 0; j < columns; j++){
      if (i == j)
        translate_matrix(i,j) = 1;
      else
        translate_matrix(i,j) = 0;
    }
    for(int i = 0; i < columns-1; i++)
      translate_matrix(rows-1,i) = vect[i];
}

template <int rows, int columns>
void scale_matrix(Coordinate factor, Matrix& scale_matrix) {
  for(int i = 0; i < rows; i++)
    for (int j = 0; j < columns; j++){
      if(i == j)
        scale_matrix(i,j) = factor[j];
      else
        scale_matrix(i,j) = 0;
    }
}

template <int rows, int columns>
void rotate_matrix(double angle, Matrix& rotate_matrix) {
  angle = (angle * PI / 180);
  for(int i = 0; i < rows; i++)
    for(int j = 0; j < columns; j++)
      rotate_matrix(i,j) = 0;
  rotate_matrix(rows-1,columns-1) = 1;
  rotate_matrix(0,0) = cos(angle); rotate_matrix(1,1) = rotate_matrix(0,0);
  rotate_matrix(1,0) = sin(angle); rotate_matrix(0,1) = -rotate_matrix(1,0);
}

template <int r1, int c1, int r2, int c2>
void matrix_multiplication(Matrix a, Matrix b, Matrix& result) {
  for(int i = 0; i < r1; i++)
    for(int j=0; j < c1; j++)
      result(i,j) = 0.0;
  for(int i = 0; i < r1; i++)
    for(int j = 0; j < c2; j++)
      for (int k = 0; k < c1; k++){
        result(i,j) += a(i,k) * b(k,j);
      }
}

};
