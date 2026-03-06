#include "Game.h"
#include"SceneMain.h"
#include"SceneTitle.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <fstream>
Game::Game()
{
}

void Game::saveData()
{
    std::ofstream file("asset/save.dat");
    if(!file.is_open()){
        return;
    }
    for(const auto &entry: leaderBoard)
    {
        file<<entry.first<<" "<<entry.second<<std::endl;
    }
}

void Game::loadData()
{
    std::ifstream file("asset/save.dat");
    if(!file.is_open()){
        return;
    }
    int score;
    std::string name;
    while(file>>name>>score){
        leaderBoard.insert({score,name});
    }
}

Game::~Game()
{
    saveData();
    clean();
}

void Game::run()
{
    
    while (isRunning)
    {
        auto framestart = SDL_GetTicks();
        SDL_Event event;
        handleEvents(&event);
       
        update(deltaTime);
      
        render();
        auto frameend = SDL_GetTicks();
        auto diff = frameend - framestart;
        if(diff < frameTime)
        {
            SDL_Delay(frameTime - diff);
            deltaTime = frameTime/1000.0f;
        }
        else
        {
            deltaTime = diff /1000.0f;
        }
        
    }
    
    
}

void Game::init()
{
    frameTime =1000/FPS;
    //SDL 初始化
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        isRunning = false;
    }

    //creat window
    window= SDL_CreateWindow("SDL_Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if(window==nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        isRunning = false;
    }

    //create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        isRunning = false;
    }
    
    //设置逻辑分辨率
    SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);



    //初始化SDL_image
if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
{
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
    isRunning = false;
}
//初始化SDL_mixer
if(Mix_Init(MIX_INIT_MP3|MIX_INIT_OGG) != (MIX_INIT_MP3|MIX_INIT_OGG))
{
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
    isRunning = false;
}

//打开音频设备
if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048) < 0){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
    isRunning = false;
}
//设置音效数量
Mix_AllocateChannels(32);
//音乐
Mix_VolumeMusic(MIX_MAX_VOLUME/4);
//音效
Mix_Volume(-1,MIX_MAX_VOLUME/8);


//初始化背景卷轴
nearStars.texture = IMG_LoadTexture(renderer, "asset/image/Stars-A.png");
if(nearStars.texture == nullptr){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
    isRunning = false;
}
SDL_QueryTexture(nearStars.texture, NULL, NULL,&nearStars.width, &nearStars.height);
nearStars.width/=2;
nearStars.height/=2;

farStars.texture = IMG_LoadTexture(renderer, "asset/image/Stars-B.png");
if(farStars.texture == nullptr){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
    isRunning = false;
}
farStars.speed = 20;
SDL_QueryTexture(farStars.texture, NULL, NULL,&farStars.width, &farStars.height);



//初始化SDL_ttf
if(TTF_Init() ==-1)
{
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
    isRunning = false;
}

//载入字体
titleFont = TTF_OpenFont("asset/font/VonwaonBitmap-16px.ttf", 64);
textFont = TTF_OpenFont("asset/font/VonwaonBitmap-16px.ttf", 32);
if(titleFont == nullptr || textFont == nullptr){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
    isRunning = false;
}
//载入数据
loadData();
//场景

currentScene = new SceneTitle();
currentScene->init();

}



void Game::clean()
{
    if(currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }

    if (nearStars.texture!= nullptr)
    {
       SDL_DestroyTexture(nearStars.texture);
    }
    if(farStars.texture != nullptr)
    {
        SDL_DestroyTexture(farStars.texture);
    }
    if(titleFont != nullptr)
    {
        TTF_CloseFont(titleFont);
    }
    if(textFont != nullptr)
    {
        TTF_CloseFont(textFont);
    }


    //clear SDL_image
    IMG_Quit();
    //claer SDL_mixer
    Mix_CloseAudio();
    Mix_Quit();

    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
}

void Game::changeScene(Scene* scene)
{
    if(currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
   
    currentScene = scene;
    currentScene->init();
}

void Game::handleEvents(SDL_Event *event)
{
       while (SDL_PollEvent(event))
        {
            if(event->type == SDL_QUIT)
            {
                isRunning = false;
            }
            if(event->type == SDL_KEYDOWN)
            {
                if(event->key.keysym.scancode == SDL_SCANCODE_F4)
                {
                    isFullscreen =!isFullscreen;
                    if(isFullscreen)
                    {
                        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);
                    }else{
                        SDL_SetWindowFullscreen(window,0);
                    }
                    
                }
            }
            currentScene->handleEvent(event);
        }
}

void Game::update(float deltaTime)
{
    backgroundUpdate(deltaTime);
    currentScene->update(deltaTime);
}

void Game::render()
{
     //清空
        SDL_RenderClear(renderer);
    //渲染星空背景    
        renderBackground();
        currentScene->render();
        //显示更新
        SDL_RenderPresent(renderer);
}

SDL_Point Game::renderTextCentered(std::string text, float posY, bool isTitle)
{
    SDL_Color color={255,255,255,255};
    SDL_Surface* surface=nullptr;
  
    if(isTitle)
    {
        surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
    }else{
        surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int y =static_cast<int>((getWindowHeight()-surface->h)*posY);
    SDL_Rect dstRect = {getWindowWidth()/2-surface->w/2,
                        y,
                        surface->w,
                        surface->h};
    SDL_RenderCopy(renderer,texture,NULL,&dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    return {dstRect.x+dstRect.w,y};
}

void Game::renderTextPos(std::string text, int posX, int posY,bool isLeft)
{
    SDL_Color color={255,255,255,255};
    SDL_Surface* surface=TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect;
    if(isLeft)
    {
        dstRect = {posX,posY,surface->w,surface->h};
    }else{
        dstRect = {getWindowWidth()-surface->w-posX,posY,surface->w,surface->h};

    }

    
    SDL_RenderCopy(renderer,texture,NULL,&dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Game::backgroundUpdate(float deltaTime)
{
    nearStars.offset+=deltaTime*nearStars.speed;
    if(nearStars.offset>=0){
        nearStars.offset-=nearStars.height;
    }

    farStars.offset+=deltaTime*farStars.speed;
    if(farStars.offset>=0){
        farStars.offset-=farStars.height;
    }
}

void Game::renderBackground()
{
    for(int posy = static_cast<int>(farStars.offset); posy<getWindowHeight(); posy+=farStars.height)
    {
        for(int posx = 0; posx<getWindowWidth(); posx+=farStars.width)
        {
            SDL_Rect dstRect = {posx,posy,farStars.width,farStars.height};
            SDL_RenderCopy(renderer,farStars.texture,NULL,&dstRect);
        }
        
    }
    for(int posy = static_cast<int>(nearStars.offset); posy<getWindowHeight(); posy+=nearStars.height)
    {
        for(int posx = 0; posx<getWindowWidth(); posx+=nearStars.width)
        {
            SDL_Rect dstRect = {posx,posy,nearStars.width,nearStars.height};
            SDL_RenderCopy(renderer,nearStars.texture,NULL,&dstRect);
        }
        
    }
}

void Game::insertLeaderBoard(int score, std::string name)
{
    leaderBoard.insert({score, name});
    if(leaderBoard.size()>8)
    {
        leaderBoard.erase(--leaderBoard.end());
    }
}
