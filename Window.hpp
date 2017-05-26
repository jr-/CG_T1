#include <algorithm>
#include <cmath>
#define MAX_SIZE 2000
#define MIN_SIZE 10

const double WMIN = -.96;
const double WMAX = .96;
const unsigned short UP = 0b1000;
const unsigned short DOWN = 0b0100;
const unsigned short RIGHT = 0b0010;
const unsigned short LEFT = 0b0001;

class Window : Object {
public:
    enum ClippingType {COHENSUTHERLAND, LIANGBARSKY};
    Window(double width, double height) : Object("Window", ObjectType::WINDOW), __init_width(width),
    __init_height(height), _width(width), _height(height), _angle(0), _scn_matrix(3,3) {
      _coords.emplace_back(0,0);
      _coords.emplace_back(_width, _height);
      _ncoords.emplace_back(-1,-1);
      _ncoords.emplace_back(1,1);
      double d = sqrt(pow(_width,2) + pow(_height,2));
      diagonal_sin = _height / d;
      diagonal_cos = _width / d;

      _wc = getCenter();
      generateSCNMatrix();
    }
    ~Window() {}
    void update (int angle=0);
    void move(double x, double y) { _coords[0].move(x,y); _coords[1].move(x,y); update(); };
    void zoom(double value);
    Coordinate getLowerLeftCoord() { return _ncoords[0]; }
    Coordinate getUpperRightCoord() { return _ncoords[1]; }
    double getWidth() { return _width; }
    double getHeight() { return _height; }
    void reset();
    Matrix getSCNMatrix() { return _scn_matrix; };
    vector<Object>* clipObjects(vector<Object> displayfile, ClippingType ct);
    bool clipPoint(Object obj);
    Object* clipLine(Object& obj, ClippingType type=COHENSUTHERLAND);
    Object* clipPolygon(Object obj);
    vector<Object> clipCurve(Object obj);
    void rotate(double angle, RotationType rt=CENTER, Coordinate reference=Coordinate(0.0,0.0)) {
      // Object::rotate(angle, rt, reference);
      update(angle);
    }
private:
    Object* cohenSuth(Object& obj);
    Object* liangBarsky(Object& obj);
    void generateSCNMatrix();
    const double __init_width, __init_height;
    double _width, _height, diagonal_sin, diagonal_cos;
    Matrix _scn_matrix;
    int _angle;
    Coordinate _wc;
};

void Window::reset() {
  _width = __init_width - 20;
  _height = __init_height - 20;
  _angle = 0;
  _coords.clear();
  _ncoords.clear();
  _coords.emplace_back(10,10);
  _coords.emplace_back(_width+10, _height+10);
  _ncoords.emplace_back(-1,-1);
  _ncoords.emplace_back(1,1);
  double d = sqrt(pow(_width,2) + pow(_height,2));
  diagonal_sin = _height / d;
  diagonal_cos = _width / d;
  _wc = getCenter();
  generateSCNMatrix();
}

void Window::update(int angle){
  // width and height update
  Coordinate wll = _coords[0];
  Coordinate wur = _coords[1];
  double d = sqrt(pow(wur[0] - wll[0],2) + pow(wur[1] - wll[1],2));
  _width = diagonal_cos * d;
  _height = diagonal_sin * d;
  // rotation angle update
  _angle += angle;
  // window center update;
  // WARNING HEISENBUG HERE :(
  cout << "window update:" << endl;
  cout << d << endl;
  cout << diagonal_sin << endl;
  cout << diagonal_cos << endl;
  _wc = getCenter();
  generateSCNMatrix();
}

void Window::generateSCNMatrix(){
  // scn matrix update
  Coordinate temp(-_wc[0], -_wc[1]);
  // calculate scale factor for normalization
  double nwidth = _ncoords[1][0] - _ncoords[0][0];
  double nheight = _ncoords[1][1] - _ncoords[0][1];
  Coordinate scale_factor(nwidth/_width, nheight/_height);
  Matrix rotate_matrix(3, 3), scale_matrix(3, 3), temp1_matrix(3, 3), temp2_matrix(3, 3);
  // temp1 holds translate to origin matrix
  ObjectManipulationMatrix::translate_matrix<3,3>(temp, temp1_matrix);
  // rotate matrix by _angle
  ObjectManipulationMatrix::rotate_matrix<3,3>(-_angle, rotate_matrix); // duvida: ela deve ser gerada com o novo angulo ou com o angulo absoluto?
  // multiply translate to origin matrix(temp1) by rotate_matrix generating a translate+rotate matrix (on temp2)
  ObjectManipulationMatrix::matrix_multiplication<3,3,3,3>(temp1_matrix, rotate_matrix, temp2_matrix);
  // generate scale_matrix
  ObjectManipulationMatrix::scale_matrix<3,3>(scale_factor, scale_matrix);
  // multiply translate+rotate matrix(temp2) by scale_matrix generating translate+rotate+scale matrix (on temp1)
  ObjectManipulationMatrix::matrix_multiplication<3,3,3,3>(temp2_matrix, scale_matrix, _scn_matrix);

}

void Window::zoom(double value) {
    bool max = _width + value > MAX_SIZE || _height + value > MAX_SIZE;
    bool min = _width + value < MIN_SIZE || _height + value < MIN_SIZE;
    if (!min && !max){
        _coords[0].move(value,value);
        _coords[1].move(-value,-value);
        update();
    }

}
//1.4
vector<Object>* Window::clipObjects(vector<Object> displayfile, ClippingType ct){
  // 1.4 TODO 1:17:20 da video aula tem como fazer o displayfile
      vector<Object>* clipped = new vector<Object>();
      Object* o = nullptr;
      vector<Object> tmp_clipped = vector<Object>();
      for( auto &obj : displayfile) {
          ObjectType type = obj.getType();
          switch(type) {
            case ObjectType::POINT:
              if(clipPoint(obj))
                clipped->push_back(obj);
              break;
            case ObjectType::LINE:
              o = clipLine(obj, ct);
              if(o != nullptr)
                clipped->push_back(*o);
              break;
            case ObjectType::POLYGON:
              o = clipPolygon(obj);
              if(o != nullptr)
                clipped->push_back(*o);
              break;
            case ObjectType::CURVE:
              tmp_clipped = clipCurve(obj);
                for( Object &obj: tmp_clipped)
                  clipped->push_back(obj);
              break;
            default:
              break;
          }
      }
      return clipped;
}

bool Window::clipPoint(Object obj) {
  vector<Coordinate> ncoords = obj.getNCoords();
  for(auto &c:ncoords) {
    if ((c[0] <= WMIN || c[0] >= WMAX) || (c[1] <= WMIN || c[1] >= WMAX))
      return false;
  }
  return true;
}


short computeRegion(double x, double y){
  short pos = 0;
  if(x < WMIN)
    pos = pos | LEFT;
  else if(x > WMAX)
    pos = pos | RIGHT;
  if (y < WMIN)
    pos = pos | DOWN;
  else if (y > WMAX)
    pos = pos | UP;
  return pos;
}


Object* Window::cohenSuth(Object& obj) {
  double x0, x1, y0, y1;
  x0 = obj.getNCoords()[0][0]; y0 = obj.getNCoords()[0][1];
  x1 = obj.getNCoords()[1][0]; y1 = obj.getNCoords()[1][1];
  unsigned short p0 = computeRegion(x0, y0);
  unsigned short p1 = computeRegion(x1, y1);
  Line* l = new Line(obj.getName());
  while((p0 | p1) != 0){
    if (p0 & p1){
      return nullptr;
    }
    double x, y;
    unsigned short outside = p0 ? p0 : p1;
    if (outside & UP) {
    		x = x0 + (x1 - x0) * (WMAX - y0) / (y1 - y0);
    		y = WMAX;
    	} else if (outside & DOWN) {
    		x = x0 + (x1 - x0) * (WMIN - y0) / (y1 - y0);
    		y = WMIN;
    	} else if (outside & RIGHT) {
        y = y0 + (y1 - y0) * (WMAX - x0) / (x1 - x0);
    		x = WMAX;
    	} else if (outside & LEFT) {
        y = y0 + (y1 - y0) * (WMIN - x0) / (x1 - x0);
    		x = WMIN;
    	}
    if (outside == p0){
      x0 = x;
      y0 = y;
      p0 = computeRegion(x0, y0);
    } else {
      x1 = x;
      y1 = y;
      p1 = computeRegion(x1, y1);
    }
  }
  l->addNCoordinate(x0, y0);
  l->addNCoordinate(x1, y1);
  return l;
}

Object* Window::liangBarsky(Object& obj) {
  double x0, y0, x1, y1;
  x0 = obj.getNCoords()[0][0]; y0 = obj.getNCoords()[0][1];
  x1 = obj.getNCoords()[1][0]; y1 = obj.getNCoords()[1][1];

  double p1 = -(x1-x0);  // -∆x
  double p2 = -p1;       // ∆x
  double p3 = -(y1-y0);  // -∆y
  double p4 = -p3;       // ∆y

  double q1 = x0-(WMIN); // x0 - wxmin
  double q2 = (WMAX) - x0; // wxmax - x0
  double q3 = y0 - (WMIN); // y0 - wymin
  double q4 = (WMAX) - y0;  // wymax - y0

  if((p1 == 0 && q1 < 0) || (p2 ==0 && q2 < 0) || (p3 == 0 && q3 < 0) || (p4 == 0 && q4 < 0)) {//fora dos limites
    return nullptr;
  }
  double r1(q1/p1), r2(q2/p2), r3(q3/p3), r4(q4/p4);
  double u1(0), u2(1);

  if (p1 < 0)
    u1 = (r1 > u1) ? r1 : u1;
  else if (p1 > 0)
    u2 = (r1 < u2) ? r1 : u2;
  if (p2 < 0)
    u1 = (r2 > u1) ? r2 : u1;
  else if (p2 > 0)
    u2 = (r2 < u2) ? r2 : u2;
  if (p3 < 0)
    u1 = (r3 > u1) ? r3 : u1;
  else if (p3 > 0)
    u2 = (r3 < u2) ? r3 : u2;
  if (p4 < 0)
    u1 = (r4 > u1) ? r4 : u1;
  else if (p4 > 0)
    u2 = (r4 < u2) ? r4 : u2;

  if (u1 > u2)
    return nullptr;

  double xn0(x0), yn0(y0), xn1(x1), yn1(y1);

  if (u1 > 0 && u1 <= 1) {
    xn0 = x0 + u1 * p2;
    yn0 = y0 + u1 * p4;
  }
  if (u2 < 1 && u2 >= 0) {
    xn1 = x0 + u2 * p2;
    yn1 = y0 + u2 * p4;
  }

  Line* l = new Line(obj.getName());
  l->addNCoordinate(xn0, yn0);
  l->addNCoordinate(xn1, yn1);

  return l;
}

Object* Window::clipLine(Object& obj, ClippingType type){
  if (type == ClippingType::COHENSUTHERLAND)
    return cohenSuth(obj);
  else
    return liangBarsky(obj);
}

Object* Window::clipPolygon(Object obj){
  Coordinate v1, v2;
  bool v1in, v2in;
  double x0, x1, y0, y1, x, y, edge;
  vector<Coordinate> ncoords = obj.getNCoords();
  // ncoords.push_back(ncoords.front());
  vector<Coordinate> newcoords{};
  Object *temp;
  short region = 0;
  for(int i = 0; i<4 ; i++) {
    region = 0b0001 << i; // change region each iteration
    edge = ((UP | RIGHT) & region) ? WMAX : WMIN; // check if the edge is maximum or minimun value of window
    ncoords.push_back(ncoords.front()); //
    for(int index = 0; index < (ncoords.size()-1); index++){
      if(ncoords.size() < 2) break;
      v1 = ncoords[index]; x0 = v1[0]; y0 = v1[1];
      v2 = ncoords[index+1]; x1 = v2[0]; y1 = v2[1];
      v1in = (computeRegion(x0, y0) & region) ? false : true;
      v2in = (computeRegion(x1, y1) & region) ? false : true;
      if (v1in && v2in) {
        newcoords.push_back(v2);
      } else if ((v1in && !v2in) || (!v1in && v2in)) {
        if ((UP | DOWN) & region){
      		x = x0 + (x1 - x0) * (edge - y0) / (y1 - y0);
      		y = edge;
        }
        if ((RIGHT | LEFT) & region){
          y = y0 + (y1 - y0) * (edge - x0) / (x1 - x0);
        	x = edge;
        }
        newcoords.emplace_back(x,y);
        if (!v1in && v2in) newcoords.emplace_back(x1,y1);
      }
    }
    ncoords = vector<Coordinate>(newcoords);
    newcoords = vector<Coordinate>();
  }
  cout << "size coords: " << ncoords.size() << endl;
  if (ncoords.size() == 0) return nullptr; // check if the edge is maximum or minimun value of window
  temp = new Polygon(obj.getName());
  for(auto& c:ncoords)
    temp->addNCoordinate(c);
  return temp;
}
//verifica se c[i] e c[i+1] estão dentro da window
//se c[i] xor c[i+1] se um estiver fora da window, faz clipLine e add c[i] na lista de coordenadas, se o c[i+1] estiver fora da window add c[i+1] na lista tambem
//se c[i] e c[i+1] estiverem fora da window, não faz nada
vector<Object> Window::clipCurve(Object obj) {
  vector<Object> objs;
  vector<Coordinate> local;
  vector<Coordinate> ncoords = obj.getNCoords();
  Line* l2 = nullptr;
  bool in = true;
  bool in2 = true;

  for(int i = 0; i < ncoords.size(); i++) {
      Point p = Point("int");
      Point p2 = Point("int2");
      p.addNCoordinate(ncoords[i]);
      p2.addNCoordinate(ncoords[i+1]);
      in = clipPoint(p);
      in2 = clipPoint(p2);

      if(in ^ in2) {
        Line lin = Line("int");
        lin.addNCoordinate(ncoords[i]);
        lin.addNCoordinate(ncoords[i+1]);
        clipLine(lin, ClippingType::COHENSUTHERLAND);
        vector<Coordinate> lNcoords = l2->getNCoords();
        local.push_back(lNcoords[0]);
        if(!in2){
          local.push_back(lNcoords[1]);
          Curve c = Curve("nome");
          for(auto &loc: local)
            c.addNCoordinate(loc);

          local.clear();
          objs.push_back(c);
        }
      } else if(in & in2) { //os 2 dentro
        local.push_back(ncoords[i]);
      }
  }
  return objs;
}
