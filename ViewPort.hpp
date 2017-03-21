#include "Window.hpp"

class ViewPort {
public:
    ViewPort(double width, double height) : _window(width,height), _width(width), _height(height) {}
    ~ViewPort() {}
    vector<Coordinate> transformObject(vector<Coordinate>);
    void move(double x, double y) { _window.move(x,y); }
    void zoom(double value) { _window.zoom(value); }

private:
    Window _window;
    double _width, _height;

};

vector<Coordinate> ViewPort::transformObject(vector<Coordinate> coords) {
    double xvp, yvp;
    double xmin, xmax, ymin, ymax;
    vector<Coordinate> coords_vp;
    Coordinate cmin = _window.getLowerLeftCoord();
    Coordinate cmax = _window.getLowerLeftCoord();
    xmin = cmin.getX(); xmax = cmax.getX();
    ymin = cmin.getY(); ymax = cmax.getY();
    for (Coordinate c : coords){
        xvp = (c.getX() - xmin) / (xmax - xmin) * (_width);
        yvp = (1 - ((c.getY() - ymin)/(ymax - ymin))) * (_height);
        coords_vp.push_back(Coordinate(xvp, yvp));
    }

    return coords_vp;
}
