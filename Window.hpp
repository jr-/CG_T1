#define MAX_SIZE 2000
#define MIN_SIZE 10

class Window : Object {
public:
    Window(double width, double height) : Object("Window", ObjectType::WINDOW), _width(width-20),
    _height(height-20), _angle(0) {
      _coords.emplace_back(10,10);
      _coords.emplace_back(_width+10, _height+10);
      _ncoords.emplace_back(-1,-1);
      _ncoords.emplace_back(1,1);
      double d = sqrt((_width**2) + (_height**2));
      diagonal_sin = _height / d;
      diagonal_cos = _width / d;
      _wc = getCenter();
      _scale_factor = Coordinate(1.0/_width,1.0/_height);
    }
    ~Window() {}

    void move(double x, double y) { wll.move(x,y); wur.move(x,y); };
    void zoom(double value);
    Coordinate getLowerLeftCoord() { return wll; }
    Coordinate getUpperRightCoord() { return wur; }
    double getWidth() { return _width; }
    double getHeight() { return _height; }
    void update (int angle);
    void rotate(double angle, RotationType rt=CENTER, Coordinate reference=Coordinate(0.0,0.0)) {
      Object::rotate(angle, rt, reference);
      update(angle);
    }

private:
    double _width, _height, diagonal_sin, diagonal_cos;
    double _scn_matrix[3][3];
    int _angle;
    Coordinate _wc, _scale_factor;
};

void Window::update(int angle=0){
  // width and height update
  Coordinate wll = _coords[0];
  Coordinate wur = _coords[1];
  double d = sqrt((wur[0] - wll[0])**2 + (wur[1] - wll[1])**2);
  _width = diagonal_cos * d;
  _height = diagonal_sin * d;
  // rotation angle update
  _angle += angle;
  // window center update;
  _wc = getCenter();

  // scn matrix update
  Coordinate temp(-_wc[0], -_wc[1]);
  double translatew[3][3], temp_matrix[3][3];
  // translate matrix to origin (1)
  ObjectManipulationMatrix::translate_matrix<3,3>(temp, translatew);
  // rotate matrix by _angle (2)
  ObjectManipulationMatrix::rotate_matrix<3,3>(-_angle, _scn_matrix); // duvida: ela deve ser gerada com o novo angulo ou com o angulo absoluto?
  // multiply translate matrix(1) by rotation matrix(2) generating temp_matrix (3)
  ObjectManipulationMatrix::matrix_multiplication<3,3,3,3>(translatew, _scn_matrix, temp_matrix);
  // translate matrix back to original place (4)
  ObjectManipulationMatrix::translate_matrix<3,3>(_wc, translatew);
  // multiply temp_matrix(3) by translate matrix(4) generating final _scn_matrix
  ObjectManipulationMatrix::matrix_multiplication<3,3,3,3>(temp_matrix, translatew, _scn_matrix);
  //calculate scale factor for normalization
  _scale_factor.set(1/_width, 1/_height);

}

void Window::zoom(double value) {
    bool max = _width + value > MAX_SIZE || _height + value > MAX_SIZE;
    bool min = _width + value < MIN_SIZE || _height + value < MIN_SIZE;
    if (!min && !max){
        _coords[0].move(value,value);
        _coords[1].move(-value,-value);
        update();
    }

};
