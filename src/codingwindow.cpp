#include "drawing.cpp"
#include "file.cpp"
#include <SDL2/SDL_render.h>
#include <iostream>
#include <ostream>
#include <string>
#include <fontconfig/fontconfig.h>
std::string font_family_to_path(const std::string& family) {
    FcInit();
    FcPattern* pat = FcPatternCreate();
    FcPatternAddString(pat, FC_FAMILY, (FcChar8*)family.c_str());
    FcConfigSubstitute(NULL, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);
    FcResult result;
    FcPattern* font = FcFontMatch(NULL, pat, &result);
    std::string path;
    if (font) {
        FcChar8* file = nullptr;
        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
            path = (char*)file;
        }
        FcPatternDestroy(font);
    }
    FcPatternDestroy(pat);
    FcFini();
    return path;
}
class CodingWindow {
    Window* window;
    int fontSize,fontIndex;
    std::string fontFamily;
    File f;
    Texture t;
    SDL_Color bg={35,33,54,255};
    SDL_Color fg={250,244,237,255};
public:
    CodingWindow(std::string filename,Window* w,int width,int height,int size,std::string family):
        f(filename,w),t(w->getRenderer(),width,height),fontSize(size),fontFamily(family) {
        fontIndex=t.loadFont(font_family_to_path(fontFamily),fontSize);
        window=w;
    }
    void update() {
        f.updateFromWindow();
    }
    void render() {
        t.clear(bg);
        t.setColor(fg);
        auto mouse=f.mousePos();
        int charWidth = 0, charHeight = 0;
        TTF_SizeText(t.getFont(fontIndex), "M", &charWidth, &charHeight);
        int lineHeight = TTF_FontLineSkip(t.getFont(fontIndex));
        t.drawRect(
            mouse.first * charWidth,
            mouse.second * lineHeight,
            2,
            lineHeight
        );
        t.drawText(f.getrope(),0,0,fontIndex,t.Width(),t.Height(),fg);
        window->drawTexture(t);
    }
};
