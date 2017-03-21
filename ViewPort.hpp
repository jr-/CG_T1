#include "Window.hpp"

class ViewPort {
public:
    ViewPort(double width, double height) : _window(width,height), _width(width), _height(height) {}
    ~ViewPort() {}
    Coordinate transformViewPort();
    void move(double x, double y) { _window.move(x,y); }
    void zoom(double value) {_window.zoom(value); }

private:
    Window _window;
    double _width, _height;

};

vector<Coordinate> transformViewPort(vector<Coordinate> coords) {
    double xvp, yvp;
    for (Coordinate c : coords)
        break;
    return coords;
}
