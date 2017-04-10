extern const double PI;

class ViewPort {
public:
    ViewPort(double width, double height) : _window(width,height), _width(width), _height(height) {}
    ~ViewPort() {}
    vector<Coordinate> transformObject(vector<Coordinate>);
    void drawObjects(vector<Object>, cairo_t *cr);
    void drawPoint(vector<Coordinate> coords, cairo_t *cr);
    void drawLine(vector<Coordinate> coords, cairo_t *cr);
    void drawPolygon(vector<Coordinate> coords, cairo_t *cr);
    void move(double x, double y) { _window.move(x,y); }
    void zoom(double value) { _window.zoom(value); }
    void rotate(int angle) { _window.rotate(angle); }

private:
    Window _window;
    double _width, _height;

};

vector<Coordinate> ViewPort::transformObject(vector<Coordinate> coords) {
    double xvp, yvp;
    double xmin, xmax, ymin, ymax;
    vector<Coordinate> coords_vp;
    Coordinate cmin = _window.getLowerLeftCoord();
    Coordinate cmax = _window.getUpperRightCoord();
    xmin = cmin.getX(); xmax = cmax.getX();
    ymin = cmin.getY(); ymax = cmax.getY();
    for (Coordinate c : coords){
        xvp = (c.getX() - xmin) / (xmax - xmin) * (_width);
        yvp = (1 - (c.getY() - ymin)/(ymax - ymin)) * (_height);
        coords_vp.push_back(Coordinate(xvp, yvp));
    }

    return coords_vp;
}

void ViewPort::drawObjects(vector<Object> displayfile, cairo_t *cr){

    for( auto &obj : displayfile) {
        ObjectType type = obj.getType();
        switch(type) {
            case ObjectType::POINT:
                drawPoint(obj.getCoords(), cr);
                break;
            case ObjectType::LINE:
                drawLine(obj.getCoords(), cr);
                break;
            case ObjectType::POLYGON:
                drawPolygon(obj.getCoords(), cr);
                break;
            default:
              break;
        }
    }
}

void ViewPort::drawPoint(vector<Coordinate> cs, cairo_t *cr) {
    vector<Coordinate> coords = transformObject(cs);
    cairo_arc(cr, coords[0].getX(), coords[0].getY(), 1.0, 0.0, (2*PI) );
    cairo_stroke(cr);
}

void ViewPort::drawLine(vector<Coordinate> c, cairo_t *cr) {
    vector<Coordinate> coords = transformObject(c);
    cairo_move_to(cr, coords[0].getX(), coords[0].getY());
    cairo_line_to(cr, coords[1].getX(), coords[1].getY());
    cairo_stroke(cr);
}

void ViewPort::drawPolygon(vector<Coordinate> c, cairo_t *cr) {
    vector<Coordinate> coords = transformObject(c);
    cairo_move_to(cr, coords[0].getX(), coords[0].getY());
    for(int i = 1; i < coords.size(); i++) {
      cairo_line_to(cr, coords[i].getX(), coords[i].getY());
    }
    cairo_line_to(cr, coords[0].getX(), coords[0].getY());
    cairo_stroke(cr);
}
