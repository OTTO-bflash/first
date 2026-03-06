#ifndef OBJECT_H
#define OBJECT_H
#include <SDL.h>

enum class ItemType{
    Health,
    Spread,
    Shield,
    Speed,
    Time,
    CoolDown
};

struct Player{
    SDL_Texture* texture=nullptr;
    SDL_Texture* Shieldtexture=nullptr;
    SDL_FPoint position={0,0};
    bool isSpreadshot=false;
    float spreadTimer=0.0f;
    float spreadDuration=5.0f;
    bool hasShield=false;
    float shieldTimer=0.0f;
    float shieldDuration=5.0f;
    int width=0;
    int height=0;
    int Shieldwidth=0;
    int Shieldheight=0;
    int speed=400;
    int currentHealth=3;
    int maxHealth=3;
    Uint32 coolDown=200 ;//ms
    Uint32 lastShootTime=0;
};

struct ProjectilePlayer{
    SDL_Texture* texture=nullptr;
    SDL_FPoint position={0,0};
    SDL_FPoint direction={0,0};
    int width=0;
    int height=0;
    int speed=600;
    int damage=1;
    double angle=0.0;
};

struct Enemy{
    SDL_Texture* texture=nullptr;
    SDL_FPoint position={0,0};
    int width=0;
    int height=0;
    int speed=150;
    int currentHealth=1;
    Uint32 coolDown=2000 ;//ms
    Uint32 lastShootTime=0;
};
struct ProjectileEnemy{
    SDL_Texture* texture=nullptr;
    SDL_FPoint position={0,0};
    SDL_FPoint direction={0,0};
    int width=0;
    int height=0;
    int speed=400;
    int damage=1;
};

struct Explosion{
    SDL_Texture* texture=nullptr;
    SDL_FPoint position={0,0};
    int width=0;
    int height=0;
    int currentFrame=0;
    int totalFrame=0;
    Uint32 startTime=0;
    Uint32 FPS = 10;
};

struct Item{
    SDL_Texture* texture=nullptr;
    SDL_FPoint position={0,0};
    SDL_FPoint direction={0,0};
    int width=0;
    int height=0;
    int speed=200;
    int bounceCount=3;
    ItemType type= ItemType::Health;
    

};
struct Background
{
   SDL_Texture* texture=nullptr;
   SDL_FPoint position={0,0};
   float offset=0;
   int width=0;
   int height=0;
   int speed=30;

};

#endif