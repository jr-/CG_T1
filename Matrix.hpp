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

  std::string toString() {
    std::string s;
    std::ostringstream strs;
    strs << _size;
    s = " Matrix size: " + strs.str() + "\n";
    strs.str("");
    for (int i=0; i<_rows; i++){
      s += "|";
      for (int j=0; j< _columns; j++){
        s += " ";
        strs << _elements.at((j + (i* _columns)));
        s += strs.str();
        strs.str("");
        s += " ";
      }
      s += "|\n";
    }
    return s;
  }

  std::vector<double> _elements;
  int _rows, _columns, _size;
};
