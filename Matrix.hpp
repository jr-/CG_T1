#include <sstream>
#include <iostream>
// template<int rows, int columns>

class Matrix {
public:
  Matrix(int rows, int columns) : _rows(rows), _columns(columns), _size(rows*columns) {
    _elements = std::vector<double>();
    for (int i=0; i<_size; i++)
      _elements.emplace_back(0.0);
  }
  ~Matrix() {}

  inline double& operator()(int row, int column) {
    return _elements.at((column + (row * _columns)));
  }

  // void print() {
  //   for (int i=0; i<_rows; i++){
  //     for (int j=0; j< _columns; j++){
  //       std::cout << _elements.at((j + (i* _columns))) << " ";
  //     }
  //   std::cout << std::endl;
  //   }
  // }

  std::vector<double> _elements;
  int _rows, _columns, _size;
};
