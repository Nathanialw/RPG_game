#pragma once
#include "timer.h"
#include "graphics.h"
#include "SDL2/SDL.h"
#include "SDL_FontCache.h"
#include "base_structs.h"
#include "events.h"
#include "pause.h"

namespace Menu
{
//    dispay buttons on screen within a context menu
//    when you click on a button it triggers an function that changes some state

//    components
    struct Size {
        i2 x, y;
        i2 w, h;
    };

//    item list that holds the button structs, needs to be a set size for each instance of its use


    //get the instance of the meni from a key, not neccessarily a string

    void Overlay()
    {
        SDL_SetRenderDrawBlendMode(Graphics::renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(Graphics::renderer, 0, 0, 0, 75);
        SDL_RenderFillRectF(Graphics::renderer, &Graphics::resolution);
    }
//
//    Button Create_Button (Size size, std::string text)
//    {
//        Button button;
//        button.size = size;
//        button.text = text;
//    }
//
//    std::string text[4] = {"ddd", "aaa", ";;;", "lll"};
//
//    void Create_Menu (Size size, SDL_Texture *texture)
//    {
//        Menu menu;
//        menu.size = size;
//
//
//    }

    SDL_Color colors[2] = {{255, 255, 255}, {255, 0,   0}};

    struct Button
    {
        SDL_FRect size;
        SDL_Surface* textSurface;
        SDL_Texture* backgroundTexture;
        SDL_Texture* textTexture;
        const char* text;
        bool selected = false;
    };

    struct Menu {
        SDL_Texture *background;
        Uint8 spacing = Mouse::cursorRadius * 2;
        std::vector<Button> buttons;
    };

//    stores menus, access with key
    std::unordered_map<std::string, Menu> menus;



    void Create_Menu(Component::Camera &camera) {
        Menu menu;
        Button button;
        std::vector<const char*>labels = {"Continue", "Exit"};
        menu.buttons.reserve(labels.size());

        for (int i = 0; i < labels.size(); i++)
        {
            menu.buttons[i].textSurface = TTF_RenderText_Solid(Graphics::font, labels[i], colors[0]);
        }
//        set first index
        menu.buttons[0].size.x = (camera.screen.w / 2) - (menu.buttons[0].textSurface->clip_rect.w / 2);
        menu.buttons[0].size.y = (camera.screen.h / 4) - (menu.buttons[0].textSurface->clip_rect.h / 2);
        menu.buttons[0].size.w = menu.buttons[0].textSurface->clip_rect.w;
        menu.buttons[0].size.h = menu.buttons[0].textSurface->clip_rect.h;
//        offset rest
        for (int i = 1; i < labels.size(); i++)
        {
            menu.buttons[i].size.x = (camera.screen.w / 2) - (menu.buttons[i-1].textSurface->clip_rect.w / 2);
            menu.buttons[i].size.y = menu.buttons[i-1].size.y + menu.buttons[i-1].size.h + menu.spacing;
            menu.buttons[i].size.w = menu.buttons[i].textSurface->clip_rect.w;
            menu.buttons[i].size.h = menu.buttons[i].textSurface->clip_rect.h;
        }

        menus["menu"] = menu;
    }

    bool toggleMenu = false;
    void Toggle(Component::Camera &camera)
    {
        if (!toggleMenu)
        {
            if (!menus.contains("menu"))
            {
                Create_Menu(camera);
            }
            toggleMenu = true;
            if (!Pause::paused) { Pause::Toggle(); }
        }
        else
        {
            toggleMenu = false;
            Pause::Toggle();
        }
    }

    int Show_Menu(Component::Camera &camera)
    {
//        const int NUMMENU = 2;
//        const char *labels[NUMMENU] = {"Continue", "Exit"};
//        SDL_Surface *menu[NUMMENU];
//        bool selected[NUMMENU] = {0, 0};
//        SDL_Color color[2] = {{255, 255, 255},
//                              {255, 0,   0}};
//        int spacing = Mouse::cursorRadius * 2;
//
//        menu[0] = TTF_RenderText_Solid(Graphics::font, labels[0], color[0]);
//        menu[1] = TTF_RenderText_Solid(Graphics::font, labels[1], color[0]);
//        SDL_FRect pos[NUMMENU];
//        pos[0].x = (camera.screen.w / 2) - (menu[0]->clip_rect.w / 2);
//        pos[0].y = (camera.screen.h / 4) - (menu[0]->clip_rect.h / 2);
//        pos[0].w = menu[0]->clip_rect.w;
//        pos[0].h = menu[0]->clip_rect.h;
//
//        pos[1].x = (camera.screen.w / 2) - (menu[1]->clip_rect.w / 2);
//        pos[1].y = pos[0].y + pos[0].h + spacing;
//        pos[1].w = menu[1]->clip_rect.w;
//        pos[1].h = menu[1]->clip_rect.h;

        Menu &menu = menus["menu"];

        for (int i = 0; i < menu.buttons.size(); i++)
        {
            if (Mouse::FRect_inside_Screen_Cursor(menu.buttons[i].size))
            {
                if (!menu.buttons[i].selected)
                {
                    menu.buttons[i].selected = 1;
                    menu.buttons[i].textSurface = TTF_RenderText_Solid(Graphics::font, menu.buttons[i].text, colors[1]);
                    SDL_FreeSurface(menu.buttons[i].textSurface);
                    menu.buttons[i].textTexture = SDL_CreateTextureFromSurface(Graphics::renderer, menu.buttons[i].textSurface);
                }

            }
            else
            {
                if (menu.buttons[i].selected)
                {
                    menu.buttons[i].selected = 0;
                    menu.buttons[i].textSurface = TTF_RenderText_Solid(Graphics::font, menu.buttons[i].text, colors[0]);
                    SDL_FreeSurface(menu.buttons[i].textSurface);
                    menu.buttons[i].textTexture = SDL_CreateTextureFromSurface(Graphics::renderer, menu.buttons[i].textSurface);
                }
            }
        }

        while (SDL_PollEvent(&Events::event))
        {
            switch (Events::event.type)
            {
                case SDL_QUIT: {
                    for (int i = 0; i < menu.buttons.size(); i++)
                    {
                        SDL_FreeSurface(menu.buttons[i].textSurface);
                    }
                    return 1;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    for (int j = 0; j < menu.buttons.size(); j++)
                    {
                        if (Mouse::FRect_inside_Screen_Cursor(menu.buttons[j].size))
                        {
//                            returns the index of the array the mouse has clicked on
                            return j;
                        }
                    }
                    break;
                }

                case SDL_KEYDOWN:
                {
                    if (Events::event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        Toggle(camera);
                        break;
                    }
                    break;
                }
            }
        }

//      render menu
        for (int i = 0; i < menu.buttons.size(); i++)
        {
//            background texture first
//            SDL_RenderCopyF(Graphics::renderer, menu.buttons[i].backgroundTexture, NULL, &menu.buttons[i].size);
            SDL_RenderCopyF(Graphics::renderer, menu.buttons[i].textTexture, NULL, &menu.buttons[i].size);
        }
        return 3;
    }

    void Render_Menu(Component::Camera &camera)
    {
        //pause with no inout
        if (toggleMenu)
        {
            Overlay();
            int i = Show_Menu(camera);
            if (i == 0)
            {
                Toggle(camera);
            }
            if (i == 1)
            {
//                exit program
                Graphics::closeContext();
            }
        }
        //toggle pause with input
    }

//    void Init_Menus(Com)
//    {
//        Create_Menu();
//    }
}