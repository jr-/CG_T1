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

void Object::print() {
  cout << "Object:" << _type << " " << _name << endl;
  cout << "-------" << endl;
  for(auto &c : _coords) {
    cout << c[0] << "  " << c[1] << endl;
  }
  cout << "-------" << endl;
  for(auto &c : _ncoords) {
    cout << c[0] << "  " << c[1] << endl;
  }
}

string Object::getName(){
  return _name;
}

ObjectType Object::getType() const {
  return _type;
}

vector<Coordinate>& Object::getCoords() {
  return _coords;
}

vector<Coordinate>& Object::getNCoords() {
  return _ncoords;
}

int Object::getSize() const {
  return _coords.size();
}

void Object::addCoordinate(double x, double y) {
  _coords.emplace_back(x, y);
}

void Object::addNCoordinate(double x, double y) {
  _ncoords.emplace_back(x, y);
}

void Object::addNCoordinate(Coordinate c){
  _ncoords.emplace_back(c[0], c[1]);
}

void Object::updateNCoordinate(Matrix scn_matrix){
  //TODO
  _ncoords.clear();
  Matrix a(1,3), result(1,3);
  for (Coordinate &c: _coords) {
      a(0,0) = c[0]; a(0,1) = c[1]; a(0,2) = c[2];
      ObjectManipulationMatrix::matrix_multiplication<1,3,3,3>(a, scn_matrix, result);
      _ncoords.emplace_back(result(0,0), result(0,1));
  }
}

void Object::translate(Coordinate vect) {
    Matrix a(1,3), result(1,3),  b(3,3);
    ObjectManipulationMatrix::translate_matrix<3,3>(vect, b);
    for (Coordinate &c: _coords) {
        a(0,0) = c[0]; a(0,1) = c[1]; a(0,2) = c[2];
        ObjectManipulationMatrix::matrix_multiplication<1,3,3,3>(a, b, result);
        c.set(result(0,0), result(0,1));
    }
}

void Object::scale(Coordinate factor) {
  Matrix a(1,3), result(1,3), b(3,3);
  ObjectManipulationMatrix::scale_matrix<3,3>(factor, b);
  Coordinate center = getCenter();
  center.set(-center.getX(), -center.getY());
  translate(center);

  for (Coordinate &c: _coords) {
    a(0,0) = c[0]; a(0,1) = c[1]; a(0,2) = c[2];
    ObjectManipulationMatrix::matrix_multiplication<1,3,3,3>(a,b, result);
    c.set(result(0,0), result(0,1));
  }
  center.set(-center.getX(), -center.getY());
  translate(center);
}

void Object::rotate(double angle, RotationType rt, Coordinate reference) {
  Matrix a(1,3), result(1,3), b(3,3);
  ObjectManipulationMatrix::rotate_matrix<3,3>(angle, b);
  switch (rt) {
    case ORIGIN:
    reference.set(0.0,0.0);
    break;
    case CENTER:
    reference = getCenter();
    break;
    case POINT:
    break;
    default:
    return;
  }
  reference.set(-reference.getX(), -reference.getY());
  translate(reference);
  reference.set(-reference.getX(), -reference.getY());
  for (Coordinate &c:_coords) {
    a(0,0) = c[0]; a(0,1) = c[1]; a(0,2) = c[2];
    ObjectManipulationMatrix::matrix_multiplication<1,3,3,3>(a, b, result);
    c.set(result(0,0), result(0,1));
  }
  translate(reference);
}

vector<Coordinate>& Curve::getControlPoints() {
    return controlPoints;
}

void Curve::addControlPoint(double x, double y) {
    controlPoints.emplace_back(x, y);
}

void Curve::generateCurve() {
    int nCurves = (controlPoints.size()-4)/3 +1;
    _coords.clear();
    auto cp = controlPoints;

    for(int i = 0; i < nCurves; i++) {
      for(double t = 0; t < 1; t += tRlx) {
          double x, y;

          double l1, l2, l3, l4;
          double t2 = t * t;
          double t3 = t2 * t;
          l1 = (-1*t3 +3*t2 -3*t +1);
          l2 = (3*t3 -6*t2 +3*t + 0);
          l3 = (-3*t3 +3*t2);
          l4 = (1*t3);

          x = cp[i*3][0]*l1 + cp[i*3+1][0]*l2 +
              cp[i*3+2][0]*l3 + cp[i*3+3][0]*l4;

          y = cp[i*3][1]*l1 + cp[i*3+1][1]*l2 +
              cp[i*3+2][1]*l3 + cp[i*3+3][1]*l4;

          _coords.emplace_back(x, y);

      }
    }
    cout << _coords.size() << endl;
}

vector<Coordinate>& BSplineCurve::getControlPoints() {
    return controlPoints;
}

void BSplineCurve::addControlPoint(double x, double y) {
    controlPoints.emplace_back(x, y);
}

void BSplineCurve::generateCurve() {
    _coords.clear();
    int nCurves = controlPoints.size()-3;
    auto cp = controlPoints;

    //0 Definir deltinha e deltinha2 deltinha3
    double t = deltinha;
    double t2 = t * t;
    double t3 = t2 * t;

    double m1 = 1.0/6.0;
    double m2 = 2.0/3.0;

    for(int i = 0; i < nCurves; i++) {
      Coordinate cp1 = cp[i];
      Coordinate cp2 = cp[i+1];
      Coordinate cp3 = cp[i+2];
      Coordinate cp4 = cp[i+3];

      //1 Calcular coeficientes
      double ax = - m1 * cp1[0] + 0.5 * cp2[0] - 0.5 * cp3[0] + m1 * cp4[0];
      double bx =  0.5 * cp1[0] + m1  * cp2[0] + 0.5 * cp3[0];
      double cx = -0.5 * cp1[0] + 0.5 * cp3[0];
      double dx =   m1 * cp1[0] + m2  * cp2[0] + m1  * cp3[0];

      double ay = - m1 * cp1[1] + 0.5 * cp2[1] - 0.5 * cp3[1] + m1 * cp4[1];
      double by =  0.5 * cp1[1] + m1  * cp2[1] + 0.5 * cp3[1];
      double cy = -0.5 * cp1[1] + 0.5 * cp3[1];
      double dy =   m1 * cp1[1] + m2  * cp2[1] + m1  * cp3[1];

      //2 Calcular delta, derivadas f0 = d d1f0 d2f0 d3f0 para o primeiro ponto da curva P1
      double d1x = ax*t3 + bx*t2 + cx*t;
      double d2x = 6*ax*t3 + 2*bx*t2;
      double d3x = 6*ax*t3;

      double d1y = ay*t3 + by*t2 + cy*t;
      double d2y = 6*ay*t3 + 2*by*t2;
      double d3y = 6*ay*t3;

      _coords.emplace_back(dx, dy);
      //3 Chamar o desenha curvafwddiff para os outros pontos
      double oldx = dx;
      double oldy = dy;
      for(double t = 0.0; t < 1.0; t += deltinha) {
        double x = oldx;
        double y = oldy;

        x += d1x;
        d1x += d2x;
        d2x += d3x;

        y += d1y;
        d1y += d2y;
        d2y += d3y;

        _coords.emplace_back(x, y);
        oldx = x;
        oldy = y;
      }
    }
    cout << _coords.size() << endl;
}
