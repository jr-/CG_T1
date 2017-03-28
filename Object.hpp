#include <string>
#include <vector>
#include <math.h>
using namespace std;
extern const double PI;

class Coordinate {
  public:
      Coordinate(double x, double y) : _x(x), _y(y) {
      }
      Coordinate(Coordinate* c) { _x = c->_x; _y = c->_y;}
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

void translateObject(Object& o, Coordinate vect);
void scaleObject(Object& o, Coordinate factor);
void rotateObject(Object& o,  double angle, RotationType rt, Coordinate reference);
template <int rows, int columns>
void _print_matrix(double matrix[rows][columns]);

template <int rows, int columns>
void _translate_matrix(Coordinate vect, double translate_matrix[rows][columns]){
  for(int i = 0; i < rows; i++)
    for (int j= 0; j < columns; j++){
      if (i == j)
        translate_matrix[i][j] = 1;
      else
        translate_matrix[i][j] = 0;
    }
    for(int i = 0; i < columns-1; i++)
      translate_matrix[rows-1][i] = vect[i];
}

template <int rows, int columns>
void _scale_matrix(Coordinate factor, double scale_matrix[rows][columns]) {
  for(int i = 0; i < rows; i++)
    for (int j = 0; j < columns; j++){
      if(i == j)
        scale_matrix[i][j] = factor[j];
      else
        scale_matrix[i][j] = 0;
    }
}

template <int rows, int columns>
void _rotate_matrix(double angle, double rotate_matrix[rows][columns]) {
  angle = (angle * PI / 180);
  for(int i = 0; i < rows; i++)
    for(int j = 0; j < columns; j++)
      rotate_matrix[i][j] = 0;
  rotate_matrix[rows-1][columns-1] = 1;
  rotate_matrix[0][0] = cos(angle); rotate_matrix[1][1] = rotate_matrix[0][0];
  rotate_matrix[1][0] = sin(angle); rotate_matrix[0][1] = -rotate_matrix[1][0];
}

template <int r1, int c1, int r2, int c2>
void _matrix_multiplication(double a[r1][c1], double b[r2][c2], double result[r1][c1]) {
  for(int i = 0; i < r1; i++)
    for(int j=0; j< c1; j++)
      result[i][j] = 0.0;
  for(int i = 0; i < r1; i++)
    for(int j = 0; j < c2; j++)
      for (int k = 0; k<c1; k++)
        result[i][j] += a[i][k] * b[k][j];
}

void _translateToOrigin(Object& o) {
  Coordinate center = o.getCenter();
  center.set(-center.getX(), -center.getY());
  translateObject(o, center);
}

void translateObject(Object& o, Coordinate vect) {
  vector<Coordinate> coords = o.getCoords();
  double a[1][3]; double b[3][3]; double result[1][3];
  _translate_matrix<3,3>(vect, b);
  for (Coordinate c: coords) {
    a[0][0] = c[0]; a[0][1] = c[1]; a[0][2] = c[2];
    _matrix_multiplication<1,3,3,3>(a, b, result);
    c.set(result[0][0], result[0][1]);
  }
}

void scaleObject(Object& o, Coordinate factor) {
  double a[1][3]; double b[3][3]; double result[1][3];
  Coordinate center = o.getCenter();
  cout << "center: " << center.getX() << ", " << center.getY() << endl;
  vector<Coordinate> coords;
  _translateToOrigin(o);
  _scale_matrix<3,3>(factor, b);
  coords = o.getCoords();
  for (Coordinate c: coords) {
    a[0][0] = c[0]; a[0][1] = c[1]; a[0][2] = c[2];
    _matrix_multiplication<1,3,3,3>(a,b, result);
    c.set(result[0][0], result[0][1]);
  }
  translateObject(o, center);
}

void rotateObject(Object& o,  int angle, RotationType rt, Coordinate reference) {
  double a[1][3]; double b[3][3]; double result[1][3];
  vector<Coordinate> coords;
  Coordinate temp(&reference);
  switch (rt) {
    case ORIGIN:
    reference.set(0.0,0.0);
    break;
    case POINT:
    temp.set(-temp.getX(), -temp.getY());
    translateObject(o, temp);
    break;
    case CENTER:
    reference = o.getCenter();
    _translateToOrigin(o);
    break;
    default:
    cout << "Warning: default rotation entered!" << endl;
    return;
  }
  _rotate_matrix<3,3>(angle, b);
  coords = o.getCoords();
  vector<Coordinate>::iterator it;
  for(it = coords.begin(); it != coords.end(); ++it) {
    a[0][0] = (*it)[0]; a[0][1] = (*it)[1]; a[0][2] = (*it)[2];
    _matrix_multiplication<1,3,3,3>(a, b, result);
    it->set(result[0][0], result[0][1]);
    cout << "Coord aft rtt: "<< it->getX() << ", "<< it->getY() << endl;

    _print_matrix<1,3>(result);
  }
  translateObject(o, reference);
}

void _test_foo() {
Coordinate vect(50.0,50.0), factor(2.0,2.0);
Line l("line1");
l.addCoordinate(0.0, 0.0);
l.addCoordinate(0.0, 100.0);

for(Coordinate c: l.getCoords()){

  cout << "coordinate before manipulation: " << c[0] << ", " << c[1] << endl;
}

// translateObject(l, vect);

// scaleObject(&l, factor);

rotateObject(l, 90, CENTER, Coordinate(0,0));

for(Coordinate c: l.getCoords()){
  cout << "coordinate after manipulation: " << c[0] << ", " << c[1] << endl;
}

}

template <int rows, int columns>
void _print_matrix(double matrix[rows][columns]) {
  for (int i = 0; i < rows; i++){
    cout << "row " << i << " | ";
   for (int j = 0; j <columns; j++){
     cout << matrix[i][j] << " ";
   }
   cout << "|" << endl;
 }
}

};
