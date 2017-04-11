#define MAX_SIZE 2000
#define MIN_SIZE 10

class Window : Object {
public:
    Window(double width, double height) : Object("Window", ObjectType::WINDOW), __init_width(width),
    __init_height(height), _width(width-20), _height(height-20), _angle(0) {
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
    ~Window() {}

    void move(double x, double y) { _coords[0].move(x,y); _coords[1].move(x,y); };
    void zoom(double value);
    Coordinate getLowerLeftCoord() { return _coords[0]; }
    Coordinate getUpperRightCoord() { return _coords[1]; }
    double getWidth() { return _width; }
    double getHeight() { return _height; }
    void reset();
    void update (int angle);
    double** getSCNMatrix() { return _scn_matrix; };
    void rotate(double angle, RotationType rt=CENTER, Coordinate reference=Coordinate(0.0,0.0)) {
      Object::rotate(angle, rt, reference);
      update(angle);
    }

private:
    void generateSCNMatrix();
    const double __init_width, __init_height;
    double _width, _height, diagonal_sin, diagonal_cos;
    double _scn_matrix[3][3];
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

void Window::update(int angle=0){
  // width and height update
  Coordinate wll = _coords[0];
  Coordinate wur = _coords[1];
  double d = sqrt(pow(wur[0] - wll[0],2) + pow(wur[1] - wll[1],2));
  _width = diagonal_cos * d;
  _height = diagonal_sin * d;
  // rotation angle update
  _angle += angle;
  // window center update;
  _wc = getCenter();
  generateSCNMatrix();
}

void Window::generateSCNMatrix(){
  // scn matrix update
  Coordinate temp(-_wc[0], -_wc[1]);
  // calculate scale factor for normalization
  Coordinate scale_factor(1.0/_width, 1.0/_height);
  double rotate_matrix[3][3], scale_matrix[3][3], temp1_matrix[3][3], temp2_matrix[3][3];
  // temp1 holds translate to origin matrix
  ObjectManipulationMatrix::translate_matrix<3,3>(temp, temp1_matrix);
  // rotate matrix by _angle
  ObjectManipulationMatrix::rotate_matrix<3,3>(-_angle, rotate_matrix); // duvida: ela deve ser gerada com o novo angulo ou com o angulo absoluto?
  // multiply translate to origin matrix(temp1) by rotate_matrix generating a translate+rotate matrix (on temp2)
  ObjectManipulationMatrix::matrix_multiplication<3,3,3,3>(temp1_matrix, rotate_matrix, temp2_matrix);
  // generate scale_matrix
  ObjectManipulationMatrix::scale_matrix<3,3>(scale_factor, scale_matrix);
  // multiply translate+rotate matrix(temp2) by scale_matrix generating translate+rotate+scale matrix (on temp1)
  ObjectManipulationMatrix::matrix_multiplication<3,3,3,3>(temp2_matrix, scale_matrix, temp1_matrix);
  // temp2 holds translate back to place matrix
  ObjectManipulationMatrix::translate_matrix<3,3>(_wc, temp2_matrix);
  // multiply translate+rotate+scale matrix(temp1) by translate back matrix(temp2) generating final _scn_matrix
  ObjectManipulationMatrix::matrix_multiplication<3,3,3,3>(temp1_matrix, temp2_matrix, _scn_matrix);

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
