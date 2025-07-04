#ifndef TEXTURE
#define TEXTURE
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
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <string>
#include <iostream>
#include <vector>
#include <ext/rope>
typedef __gnu_cxx::crope rope;
inline std::string rope_substr(const rope& r, size_t start, size_t len) {
    return r.substr(start, len).c_str();
}

class Texture {
    SDL_Texture* texture = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Color curcolor;
    std::vector<TTF_Font*> fonts;
    int width,height;
public:
    Texture(SDL_Renderer* r,int width,int height): width(width),height(height),renderer(r) {
        texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,width,height);
        if (!texture) {
            std::cerr<<"SDL_CreateTexture Error: "<<SDL_GetError()<<std::endl;
        }
    }
    ~Texture() {
        for (auto i:fonts) {
            TTF_CloseFont(i);
        }
        if (texture) SDL_DestroyTexture(texture);
    }
    SDL_Texture* getTexture() {
        return texture;
    }
    int loadFont(std::string fontPath,int fontSize) {
        fonts.push_back(TTF_OpenFont(fontPath.c_str(),fontSize));
        return fonts.size()-1;
    }
    int reloadFont(int i,std::string fontPath,int fontSize) {
        if (i>=fonts.size()) {
            return loadFont(fontPath,fontSize);
        } else {
            TTF_CloseFont(fonts[i]);
            fonts[i]=TTF_OpenFont(fontPath.c_str(),fontSize);
            return i;
        }
    }
    TTF_Font* getFont(int i) {
        if (i>=fonts.size()) return nullptr;
        return fonts[i];
    }
    int Width() {
        return width;
    }
    int Height() {
        return height;
    }
    void resize(int newWidth,int newHeight) {
        if (texture) SDL_DestroyTexture(texture);
        if (!renderer) return;
        texture=SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, newWidth, newHeight);
        width=newWidth;
        height=newHeight;
    }
    void setColor(SDL_Color c) {
        SDL_SetRenderTarget(renderer,texture);
        SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a);
        curcolor=c;
        SDL_SetRenderTarget(renderer,NULL);
    }
    void setColor(Uint8 r,Uint8 g,Uint8 b,Uint8 a) {
        SDL_SetRenderTarget(renderer,texture);
        SDL_SetRenderDrawColor(renderer,r,g,b,a);
        curcolor={r,g,b,a};
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawPoint(int x,int y) {
        SDL_SetRenderTarget(renderer,texture);
        SDL_RenderDrawPoint(renderer,x,y);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawLine(int x1,int y1,int x2,int y2) {
        SDL_SetRenderTarget(renderer,texture);
        aalineRGBA(renderer,x1,y1,x2,y2,curcolor.r,curcolor.g,curcolor.b,curcolor.a);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawRect(int x,int y,int w,int h) {
        SDL_SetRenderTarget(renderer,texture);
        SDL_Rect rect = {x,y,w,h};
        SDL_RenderDrawRect(renderer,&rect);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void fillRect(int x,int y,int w,int h) {
        SDL_SetRenderTarget(renderer,texture);
        SDL_Rect rect = {x,y,w,h};
        SDL_RenderFillRect(renderer,&rect);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void fillRoundedRect(int x, int y, int w, int h, int r) {
        SDL_SetRenderTarget(renderer,texture);
        SDL_Rect rect = {x + r, y, w - 2 * r, h};
        SDL_RenderFillRect(renderer, &rect);
        SDL_Rect rect2 = {x, y + r, r, h - 2 * r}; 
        SDL_RenderFillRect(renderer, &rect2);
        SDL_Rect rect3 = {x + w - r, y + r, r, h - 2 * r}; 
        SDL_RenderFillRect(renderer, &rect3);
        fillCircle(x + r, y + r, r); 
        fillCircle(x + w - r - 1, y + r, r); 
        fillCircle(x + r, y + h - r - 1, r); 
        fillCircle(x + w - r - 1, y + h - r - 1, r); 
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawCircle(int centerX,int centerY,int radius) {
        SDL_SetRenderTarget(renderer,texture);
        circleRGBA(renderer,centerX,centerY,radius,curcolor.r,curcolor.g,curcolor.b,curcolor.a);
    }
    void fillCircle(int centerX,int centerY,int radius) {
        SDL_SetRenderTarget(renderer,texture);
        filledCircleRGBA(renderer,centerX,centerY,radius,curcolor.r,curcolor.g,curcolor.b,curcolor.a);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawArc(int x,int y,int radius,float startAngle,float endAngle) {
        SDL_SetRenderTarget(renderer,texture);
        float startRad = startAngle * (M_PI / 180.0f);
        float endRad = endAngle * (M_PI / 180.0f);
        arcRGBA(renderer,x,y,radius,startAngle,endAngle,curcolor.r,curcolor.g,curcolor.b,curcolor.a);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawPoly(const std::vector<Sint16>& px, const std::vector<Sint16>& py) {
        SDL_SetRenderTarget(renderer,texture);
        int num_points = std::min(px.size(), py.size());
        std::vector<Sint16> x(num_points);
        std::vector<Sint16> y(num_points);
        for (int i = 0; i < num_points; i++) {
            x[i] = px[i];
            y[i] = py[i];
        }
        polygonRGBA(renderer, x.data(), y.data(), num_points, curcolor.r, curcolor.g, curcolor.b, curcolor.a);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void fillPoly(const std::vector<Sint16>& px, const std::vector<Sint16>& py) {
        SDL_SetRenderTarget(renderer,texture);
        int num_points = std::min(px.size(), py.size());
        std::vector<Sint16> x(num_points);
        std::vector<Sint16> y(num_points);
        for (int i = 0; i < num_points; i++) {
            x[i] = px[i];
            y[i] = py[i];
        }
        filledPolygonRGBA(renderer, x.data(), y.data(), num_points, curcolor.r, curcolor.g, curcolor.b, curcolor.a);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawText(std::string text, int x, int y, int f, int maxWidth, SDL_Color color) {
        SDL_SetRenderTarget(renderer,texture);
        TTF_Font* font = fonts[f];
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return;
        }
        std::vector<std::string> lines;
        std::string currentLine;
        for (size_t i = 0; i < text.size(); ++i) {
            char c = text[i];
            if (c == '\n') {
                lines.push_back(currentLine);
                currentLine.clear();
                continue;
            }
            currentLine += c;
            int textWidth = 0, textHeight = 0;
            if (TTF_SizeText(font, currentLine.c_str(), &textWidth, &textHeight) != 0) {
                std::cerr << "TTF_SizeText Error: " << TTF_GetError() << std::endl;
                return;
            }
            if (textWidth > maxWidth) {
                currentLine.pop_back();
                lines.push_back(currentLine);
                currentLine = c;
            }
        }
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }
        if (!text.empty() && text.back() == '\n') {
            lines.push_back("");
        }
        int lineHeight = TTF_FontLineSkip(font);
        int yOffset = 0;
        for (const std::string& line : lines) {
            if (!line.empty()) {
                SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), color);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    if (texture) {
                        int textWidth = 0, textHeight = 0;
                        SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
                        SDL_Rect destRect = {x, y + yOffset, textWidth, textHeight};
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                        SDL_DestroyTexture(texture);
                    }
                }
            }
            yOffset += lineHeight;
        }
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawText(std::string text, int x, int y, int f, int maxWidth, int maxHeight, SDL_Color color) {
        SDL_SetRenderTarget(renderer,texture);
        TTF_Font* font = fonts[f];
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return;
        }
        std::vector<std::string> lines;
        std::string currentLine;
        int totalHeight = 0;
        int lineHeight = TTF_FontLineSkip(font);
        for (size_t i = 0; i < text.size(); ++i) {
            char c = text[i];
            if (c == '\n') {
                lines.push_back(currentLine);
                totalHeight += lineHeight;
                if (totalHeight + lineHeight > maxHeight) break;
                currentLine.clear();
                continue;
            }
            currentLine += c;
            int textWidth = 0, textHeight = 0;
            if (TTF_SizeText(font, currentLine.c_str(), &textWidth, &textHeight) != 0) {
                std::cerr << "TTF_SizeText Error: " << TTF_GetError() << std::endl;
                return;
            }
            if (textWidth > maxWidth) {
                currentLine.pop_back();
                lines.push_back(currentLine);
                totalHeight += lineHeight;
                if (totalHeight + lineHeight > maxHeight) break;
                currentLine = c;
            }
        }
        if (!currentLine.empty() && totalHeight + lineHeight <= maxHeight) {
            lines.push_back(currentLine);
        }
        if (!text.empty() && text.back() == '\n' && totalHeight + lineHeight <= maxHeight) {
            lines.push_back("");
        }
        int yOffset = 0;
        for (const std::string& line : lines) {
            if (!line.empty()) {
                SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), color);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    if (texture) {
                        int textWidth = 0, textHeight = 0;
                        SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
                        SDL_Rect destRect = {x, y + yOffset, textWidth, textHeight};
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                        SDL_DestroyTexture(texture);
                    }
                }
            }
            yOffset += lineHeight;
            if (y + yOffset + lineHeight > y + maxHeight) break;
        }
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawText(std::string text, int x, int y, const std::string& fontPath, int fontSize, int maxWidth, int maxHeight, SDL_Color color) {
        SDL_SetRenderTarget(renderer,texture);
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return;
        }
        std::vector<std::string> lines;
        std::string currentLine;
        int totalHeight = 0;
        int lineHeight = TTF_FontLineSkip(font);
        for (size_t i = 0; i < text.size(); ++i) {
            char c = text[i];
            if (c == '\n') {
                lines.push_back(currentLine);
                totalHeight += lineHeight;
                if (totalHeight + lineHeight > maxHeight) break;
                currentLine.clear();
                continue;
            }
            currentLine += c;
            int textWidth = 0, textHeight = 0;
            if (TTF_SizeText(font, currentLine.c_str(), &textWidth, &textHeight) != 0) {
                std::cerr << "TTF_SizeText Error: " << TTF_GetError() << std::endl;
                TTF_CloseFont(font);
                return;
            }
            if (textWidth > maxWidth) {
        SDL_SetRenderTarget(renderer,texture);
                currentLine.pop_back();
                lines.push_back(currentLine);
                totalHeight += lineHeight;
                if (totalHeight + lineHeight > maxHeight) break;
                currentLine = c;
            }
        }
        if (!currentLine.empty() && totalHeight + lineHeight <= maxHeight) {
            lines.push_back(currentLine);
        }
        if (!text.empty() && text.back() == '\n' && totalHeight + lineHeight <= maxHeight) {
            lines.push_back("");
        }
        int yOffset = 0;
        for (const std::string& line : lines) {
            if (!line.empty()) {
                SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), color);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    if (texture) {
                        int textWidth = 0, textHeight = 0;
                        SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
                        SDL_Rect destRect = {x, y + yOffset, textWidth, textHeight};
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                        SDL_DestroyTexture(texture);
                    }
                }
            }
            yOffset += lineHeight;
            if (y + yOffset + lineHeight > y + maxHeight) break;
        }
        TTF_CloseFont(font);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void drawText(std::string text, int x, int y, const std::string& fontPath, int fontSize, int maxWidth, SDL_Color color) {
        SDL_SetRenderTarget(renderer,texture);
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return;
        }
        int textWidth = 0, textHeight = 0;
        if (TTF_SizeText(font, text.c_str(), &textWidth, &textHeight) != 0) {
            std::cerr << "TTF_SizeText Error: " << TTF_GetError() << std::endl;
            TTF_CloseFont(font);
            return;
        }
        while (textWidth > maxWidth && !text.empty()) {
            text.pop_back();
            TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);
        }
        if (!text.empty()) {
            SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
            if (surface) {;
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                if (texture) {
                    SDL_Rect destRect = {x, y, 0, 0};
                    SDL_QueryTexture(texture, NULL, NULL, &destRect.w, &destRect.h);
                    SDL_RenderCopy(renderer, texture, NULL, &destRect);
                    SDL_DestroyTexture(texture);
                }
            }
        }
        TTF_CloseFont(font);
        SDL_SetRenderTarget(renderer,NULL);
    }

    void drawText(const rope& text, int x, int y, int f, int maxWidth, SDL_Color color) {
        SDL_SetRenderTarget(renderer, texture);
        TTF_Font* font = fonts[f];
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return;
        }
        std::vector<std::string> lines;
        std::string currentLine;
        size_t N = text.size();
        for (size_t i = 0; i < N; ++i) {
            char c = text[i];
            if (c == '\n') {
                lines.push_back(currentLine);
                currentLine.clear();
                continue;
            }
            currentLine += c;
            int textWidth = 0, textHeight = 0;
            if (TTF_SizeText(font, currentLine.c_str(), &textWidth, &textHeight) != 0) {
                std::cerr << "TTF_SizeText Error: " << TTF_GetError() << std::endl;
                return;
            }
            if (textWidth > maxWidth) {
                currentLine.pop_back();
                lines.push_back(currentLine);
                currentLine = c;
            }
        }
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }
        if (N > 0 && text[N-1] == '\n') {
            lines.push_back("");
        }
        int lineHeight = TTF_FontLineSkip(font);
        int yOffset = 0;
        for (const std::string& line : lines) {
            if (!line.empty()) {
                SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), color);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    if (texture) {
                        int textWidth = 0, textHeight = 0;
                        SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
                        SDL_Rect destRect = {x, y + yOffset, textWidth, textHeight};
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                        SDL_DestroyTexture(texture);
                    }
                }
            }
            yOffset += lineHeight;
        }
        SDL_SetRenderTarget(renderer, NULL);
    }
    
    // 2. drawText with maxWidth and maxHeight
    void drawText(const rope& text, int x, int y, int f, int maxWidth, int maxHeight, SDL_Color color) {
        SDL_SetRenderTarget(renderer, texture);
        TTF_Font* font = fonts[f];
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return;
        }
        std::vector<std::string> lines;
        std::string currentLine;
        int totalHeight = 0;
        int lineHeight = TTF_FontLineSkip(font);
        size_t N = text.size();
        for (size_t i = 0; i < N; ++i) {
            char c = text[i];
            if (c == '\n') {
                lines.push_back(currentLine);
                totalHeight += lineHeight;
                if (totalHeight + lineHeight > maxHeight) break;
                currentLine.clear();
                continue;
            }
            currentLine += c;
            int textWidth = 0, textHeight = 0;
            if (TTF_SizeText(font, currentLine.c_str(), &textWidth, &textHeight) != 0) {
                std::cerr << "TTF_SizeText Error: " << TTF_GetError() << std::endl;
                return;
            }
            if (textWidth > maxWidth) {
                currentLine.pop_back();
                lines.push_back(currentLine);
                totalHeight += lineHeight;
                if (totalHeight + lineHeight > maxHeight) break;
                currentLine = c;
            }
        }
        if (!currentLine.empty() && totalHeight + lineHeight <= maxHeight) {
            lines.push_back(currentLine);
        }
        if (N > 0 && text[N-1] == '\n' && totalHeight + lineHeight <= maxHeight) {
            lines.push_back("");
        }
        int yOffset = 0;
        for (const std::string& line : lines) {
            if (!line.empty()) {
                SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), color);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    if (texture) {
                        int textWidth = 0, textHeight = 0;
                        SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
                        SDL_Rect destRect = {x, y + yOffset, textWidth, textHeight};
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                        SDL_DestroyTexture(texture);
                    }
                }
            }
            yOffset += lineHeight;
            if (y + yOffset + lineHeight > y + maxHeight) break;
        }
        SDL_SetRenderTarget(renderer, NULL);
    }
    
    // 3. drawText with font path, font size, maxWidth, maxHeight
    void drawText(const rope& text, int x, int y, const std::string& fontPath, int fontSize, int maxWidth, int maxHeight, SDL_Color color) {
        SDL_SetRenderTarget(renderer, texture);
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return;
        }
        std::vector<std::string> lines;
        std::string currentLine;
        int totalHeight = 0;
        int lineHeight = TTF_FontLineSkip(font);
        size_t N = text.size();
        for (size_t i = 0; i < N; ++i) {
            char c = text[i];
            if (c == '\n') {
                lines.push_back(currentLine);
                totalHeight += lineHeight;
                if (totalHeight + lineHeight > maxHeight) break;
                currentLine.clear();
                continue;
            }
            currentLine += c;
            int textWidth = 0, textHeight = 0;
            if (TTF_SizeText(font, currentLine.c_str(), &textWidth, &textHeight) != 0) {
                std::cerr << "TTF_SizeText Error: " << TTF_GetError() << std::endl;
                TTF_CloseFont(font);
                return;
            }
            if (textWidth > maxWidth) {
                currentLine.pop_back();
                lines.push_back(currentLine);
                totalHeight += lineHeight;
                if (totalHeight + lineHeight > maxHeight) break;
                currentLine = c;
            }
        }
        if (!currentLine.empty() && totalHeight + lineHeight <= maxHeight) {
            lines.push_back(currentLine);
        }
        if (N > 0 && text[N-1] == '\n' && totalHeight + lineHeight <= maxHeight) {
            lines.push_back("");
        }
        int yOffset = 0;
        for (const std::string& line : lines) {
            if (!line.empty()) {
                SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), color);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    if (texture) {
                        int textWidth = 0, textHeight = 0;
                        SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
                        SDL_Rect destRect = {x, y + yOffset, textWidth, textHeight};
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                        SDL_DestroyTexture(texture);
                    }
                }
            }
            yOffset += lineHeight;
            if (y + yOffset + lineHeight > y + maxHeight) break;
        }
        TTF_CloseFont(font);
        SDL_SetRenderTarget(renderer, NULL);
    }
    
    // 4. drawText with font path, font size, maxWidth
    void drawText(const rope& text, int x, int y, const std::string& fontPath, int fontSize, int maxWidth, SDL_Color color) {
        SDL_SetRenderTarget(renderer, texture);
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return;
        }
        // For this function, we need to flatten the rope for TTF_SizeText.
        // For very large ropes, consider rendering in chunks/lines for efficiency.
        std::string s = text.c_str();
        int textWidth = 0, textHeight = 0;
        if (TTF_SizeText(font, s.c_str(), &textWidth, &textHeight) != 0) {
            std::cerr << "TTF_SizeText Error: " << TTF_GetError() << std::endl;
            TTF_CloseFont(font);
            return;
        }
        while (textWidth > maxWidth && !s.empty()) {
            s.pop_back();
            TTF_SizeText(font, s.c_str(), &textWidth, &textHeight);
        }
        if (!s.empty()) {
            SDL_Surface* surface = TTF_RenderText_Blended(font, s.c_str(), color);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                if (texture) {
                    SDL_Rect destRect = {x, y, 0, 0};
                    SDL_QueryTexture(texture, NULL, NULL, &destRect.w, &destRect.h);
                    SDL_RenderCopy(renderer, texture, NULL, &destRect);
                    SDL_DestroyTexture(texture);
                }
            }
        }
        TTF_CloseFont(font);
        SDL_SetRenderTarget(renderer, NULL);
    }
    void clear() {
        SDL_SetRenderTarget(renderer,texture);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer,NULL);
    }
    void clear(SDL_Color c) {
        SDL_SetRenderTarget(renderer,texture);
        SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer,NULL);
    }
};
#endif
