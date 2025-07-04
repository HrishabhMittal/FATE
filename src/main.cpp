#include "file.cpp"
#include "timer.cpp"
#include "codingwindow.cpp"
#include <iostream>
#include <ostream>

int main(int argc, char** argv) {
    Window window("Text Editor", 1000, 800);
    CodingWindow cw("main.cpp",&window,1000,800,20,"FiraCode");
    while (window.running) {
        //Timer t;
        window.pollEvents();
        window.clear({30, 30, 30, 255});
        cw.update();
        cw.render();
        window.present();
        setFrameRate(60);
        //int millis=t.now();
        //std::cout<<"Frame rate: "<<1000.0/millis<<std::endl;
    }
    
    return 0;
}
