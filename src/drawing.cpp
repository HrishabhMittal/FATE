#ifndef DRAWING
#define DRAWING
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_video.h>
#include <ostream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "texture.cpp"
bool operator==(const SDL_Rect& one,const SDL_Rect& other) {
    return one.x == other.x && one.y == other.y && one.w==other.w && one.h==other.h;
}
bool operator!=(const SDL_Rect& one,const SDL_Rect& other) {
    return one.x != other.x || one.y != other.y || one.w!=other.w || one.h!=other.h;
}
int integralSquareRoot(int num) {
    if (num < 0) return -1; 
    if (num == 0 || num == 1) return num;
    int low = 1, high = num, result = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (mid == num / mid) {
            return mid; 
        } else if (mid < num / mid) {
            result = mid; 
            low = mid + 1; 
        } else {
            high = mid - 1; 
        }
    }
    return result; 
}
double calcAngle(int x1, int y1, int x2, int y2) {
    return fmod(atan2(y2 - y1, x2 - x1) * (180.0 / M_PI) + 360.0, 360.0);
}
bool pointInside(SDL_Point p,SDL_Rect r) {
    if (p.x>r.x&&p.y>r.y&&p.x<r.x+r.w&&p.y<r.y+r.h) {
        return true;
    }
    return false;
}
bool collide(SDL_Rect r1,SDL_Rect r2) {
    if (r1.x+r1.w>r2.x && r2.x+r2.w>r1.x && r1.y+r1.h>r2.y && r2.y+r2.h>r1.y) return true;
    return false;
}
void setFrameRate(int targetFPS) {
    static Uint32 lastTime = 0;
    Uint32 frameTime = 1000 / targetFPS;
    Uint32 currentTime = SDL_GetTicks();
    Uint32 deltaTime = currentTime - lastTime;
    if (deltaTime < frameTime) {
        SDL_Delay(frameTime - deltaTime);
    }
    lastTime = SDL_GetTicks();
}

class Window {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Color curcolor;
    int width,height;
    //std::vector<Texture*> textures;
public:
    int mouseX,mouseY; Uint32 buttons;
    int scrollX,scrollY;
    std::unordered_map<int,int> keyspressed;
    int running=1;
    Window(const std::string& title,int width,int height): width(width),height(height) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return;
        }
        if (TTF_Init() == -1) {
            std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
            return;
        }
        if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
            std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
            return;
        }
        window = SDL_CreateWindow(title.c_str(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width,height,SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
        if (!window) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            return;
        }
        renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            return;
        }
    }
    SDL_Renderer* getRenderer() {
        return renderer;
    }
    SDL_Rect windowDimensions() {
        return {0,0,width,height};
    }
    void pollEvents() {
        SDL_Event e;
        for (auto& i:keyspressed) {
            if (i.second!=0) i.second++;
        }
        while (SDL_PollEvent(&e)) {
            if (e.type==SDL_QUIT) {
                running=false;
            }
            if (e.type==SDL_KEYDOWN) {
                if (keyspressed[e.key.keysym.sym]==0) keyspressed[e.key.keysym.sym]=1;
            } else if (e.type==SDL_KEYUP) {
                keyspressed[e.key.keysym.sym]=0;
            } else if (e.type==SDL_MOUSEWHEEL) {
                scrollX+=e.wheel.x;
                scrollY+=e.wheel.y;
            }
        }
        buttons=SDL_GetMouseState(&mouseX, &mouseY);
        SDL_GetWindowSize(window,&width,&height);
    }
    ~Window() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
    }
    void drawTexture(Texture& t,SDL_Rect img,SDL_Rect win) {
        SDL_Texture* te=t.getTexture();
        SDL_Texture* texture = te;
        SDL_RenderCopy(renderer,texture,&img,&win);
    }
    void drawTexture(Texture& t,SDL_Rect win) {
        SDL_Texture* te=t.getTexture();
        SDL_Texture* texture = te;
        SDL_Rect img{0,0,0,0};
        SDL_QueryTexture(texture,NULL,NULL,&img.w,&img.h);
        SDL_RenderCopy(renderer,texture,&img,&win);
    }
    void drawTexture(Texture& t) {
        SDL_Texture* te=t.getTexture();
        SDL_Texture* texture = te;
        SDL_Rect img{0,0,0,0};
        SDL_Rect win{0,0,width,height};
        SDL_QueryTexture(texture,NULL,NULL,&img.w,&img.h);
        SDL_RenderCopy(renderer,texture,&img,&win);
    }
    int Width() {
        return width;
    }
    int Height() {
        return height;
    }
    void present() {
        SDL_RenderPresent(renderer);
    }
    void clear() {
        SDL_RenderClear(renderer);
    }
    void clear(SDL_Color c) {
        SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a);
        SDL_RenderClear(renderer);
    }
};
#endif
