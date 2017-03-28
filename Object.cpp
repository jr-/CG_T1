#include "Object.h"

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

string Object::getName() const {
  return _name;
}

ObjectType Object::getType() const {
  return _type;
}

vector<Coordinate>& Object::getCoords() {
  return _coords;
}

int Object::getSize() const {
  return _coords.size();
}

void Object::addCoordinate(double x, double y) {
  _coords.emplace_back(new Coordinate(x,y));
}

void Object::translate(Coordinate vect) {
  double a[1][3]; double b[3][3]; double result[1][3];
  ObjectManipulationMatrix::translate_matrix<3,3>(vect, b);
  for (Coordinate &c: _coords) {
    a[0][0] = c[0]; a[0][1] = c[1]; a[0][2] = c[2];
    ObjectManipulationMatrix::matrix_multiplication<1,3,3,3>(a, b, result);
    c.set(result[0][0], result[0][1]);
  }
}

void Object::scale(Coordinate factor) {
  double a[1][3]; double b[3][3]; double result[1][3];
  ObjectManipulationMatrix::scale_matrix<3,3>(factor, b);
  Coordinate center = getCenter();
  center.set(-center.getX(), -center.getY());
  translate(center);

  for (Coordinate &c: _coords) {
    a[0][0] = c[0]; a[0][1] = c[1]; a[0][2] = c[2];
    ObjectManipulationMatrix::matrix_multiplication<1,3,3,3>(a,b, result);
    c.set(result[0][0], result[0][1]);
  }
  center.set(-center.getX(), -center.getY());
  translate(center);
}

void Object::rotate(double angle, RotationType rt, Coordinate reference) {
  double a[1][3]; double b[3][3]; double result[1][3];
  ObjectManipulationMatrix::rotate_matrix<3,3>(angle, b);
  switch (rt) {
    case ORIGIN:
    reference.set(0.0,0.0);
    break;
    case POINT:
    reference.set(-reference.getX(), -reference.getY());
    break;
    case CENTER:
    reference = getCenter();
    break;
    default:
    return;
  }
  translate(reference);
  reference.set(-reference.getX(), -reference.getY());
  for (Coordinate &c:_coords) {
    a[0][0] = c[0]; a[0][1] = c[1]; a[0][2] = c[2];
    ObjectManipulationMatrix::matrix_multiplication<1,3,3,3>(a, b, result);
    c.set(result[0][0], result[0][1]);
  }
  translate(reference);
}
