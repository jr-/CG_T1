class Coordinate {
  public:
      Coordinate() : _x(0), _y(0) {}
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
