#include <iostream>
#include <vector>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "raylib.h"
#include "types.hpp"

Map generate_terrain();
TextMap load_assets();
void draw_terrain(Map &mapa, TextMap &texturas);

void load_items(lua_State *L);
lua_State* get_luastate();
void close_lua(lua_State *L);

const int MAX_MAP_WIDTH = 512;
const int MAX_MAP_HEIGHT = 512;

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [textures] example - logo raylib");

    Map mapa = generate_terrain();
    TextMap texturas = load_assets();
    Texture2D *player = &texturas['p'];

    lua_State* L = get_luastate();
    load_items(L);
    close_lua(L);

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        draw_terrain(mapa, texturas);
        DrawTexture(texturas['p'], screenWidth/2 - player->width/2, screenHeight/2 - player->height/2, WHITE);

        DrawText("this IS a texture!", 360, 370, 10, GRAY);

        EndDrawing();
    }

    for (const auto &par : texturas) {
        UnloadTexture(par.second);
    }
    // UnloadTexture(texture);

    CloseWindow();

    return 0;
}

TextMap load_assets() {
    TextMap texturas;
    texturas['p'] = LoadTexture("assets/player.png");
    texturas['g'] = LoadTexture("assets/grass.png");
    return texturas;
}

Map generate_terrain() {

    Map mapa(MAX_MAP_WIDTH, Cell(MAX_MAP_HEIGHT, 0));

    for (int x = 0;x < MAX_MAP_WIDTH;x++) {
        for (int y = 0; y<MAX_MAP_HEIGHT;y++) {
            mapa[x][y] = 'g';
        }
    }
    return mapa;
}

void draw_terrain(Map &mapa, TextMap &texturas) {
    int w = 32;
    int h = 32;

    for (int x = 0;x < MAX_MAP_WIDTH;x++) {
        Texture2D *t = nullptr;
        for (int y = 0; y < MAX_MAP_HEIGHT;y++) {
            t = &texturas[mapa[x][y]];
            DrawTexture(*t, w*x, h*y, WHITE);
        }
    }
}

struct WorldObject {
    int id;
    std::string sprite;
};

lua_State* get_luastate() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    return L;
}

void close_lua(lua_State *L) {
        lua_close(L);
}

void load_items(lua_State *L) {

    luaL_dofile(L, "items.lua");
    std::vector<WorldObject> worlds;

    int length = static_cast<int>(lua_rawlen(L, -1));

    for (int i = 0; i<= length ; ++i) {
        lua_rawgeti(L, -1, i);

        lua_getglobal(L, "world");
        if (lua_istable(L,-1)) {
            WorldObject world;

            lua_getfield(L, -1, "id");
            world.id = static_cast<int>(lua_tointeger(L, -1));
            lua_pop(L, 1);

            lua_getfield(L, -1, "sprite");
            world.sprite = lua_tostring(L, -1);
            lua_pop(L, 1);

            worlds.push_back(world);
        }

        lua_pop(L, 1);
        lua_pop(L, 1);

        for (const auto& obj : worlds) {
            std::cout << "ID: " << obj.id 
                << " | Sprite: " << obj.sprite << std::endl;
        }
    }
}
