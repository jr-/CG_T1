#include "Coordinate.hpp"
#define MAX 2000
#define MIN 10

class Window {
public:
    Window(double width, double height) wll(0,0), wur(width, height), _width(width), _height(heigth) {}
    ~Window() {}

    void move(double x, double y) { wll.move(x,y); wur(x,y); };
    void zoom(double value);
private:
    Coordinate wll, wur;
    double _width, _height;
};

void Window::zoom(double value) {
    value = value / 2;
    bool max = _width + value > MAX || _height + value < MAX;
    bool min = _width + value < MIN || _height + value < MIN;
    if (!min && !max){
        wll.move(value,value);
        wur.move(-value,-value);
    }

};
