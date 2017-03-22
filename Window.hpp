#define MAX_SIZE 2000
#define MIN_SIZE 10

class Window {
public:
    Window(double width, double height) : wll(0,0), wur(width, height), _width(width), _height(height) {}
    ~Window() {}

    void move(double x, double y) { wll.move(x,y); wur.move(x,y); };
    void zoom(double value);
    Coordinate getLowerLeftCoord() { return wll; }
    Coordinate getUpperRightCoord() { return wur; }
    double getWidth() { return _width; }
    double getHeight() { return _height; }

private:
    Coordinate wll, wur;
    double _width, _height;
};

void Window::zoom(double value) {
    // value = value;
    std::cout << "Width: "<< _width << " Height: " << _height << '\n';

    bool max = _width + value > MAX_SIZE || _height + value > MAX_SIZE;
    bool min = _width + value < MIN_SIZE || _height + value < MIN_SIZE;
    std::cout << max << " " << min << '\n';
    if (!min && !max){
        wll.move(value,value);
        wur.move(-value,-value);
        _width = wur.getX() - wll.getX();
        _height = wur.getY() - wll.getY();
    }

};
