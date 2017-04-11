
template<int rows, int columns>
class Matrix {
public:
  Matrix() : _rows(rows), _columns(columns), _size(rows*columns) {
    _elements = vector<double>();
    for (int i=0; i<_size; i++)
      _elements.emplace_back(0.0);
  }
  ~Matrix() {}
  inline double& operator()(int row, int column) {
    return _elements.at((column + (row * _columns)));
  }

private:
  vector<double> _elements;
  int _rows, _columns, _size;
};
