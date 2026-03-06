#include "SceneTitle.h"
#include "SceneMain.h"
#include <string>
#include "Game.h"
void SceneTitle::init()
{
    //载入并播放背景音乐
    bgm = Mix_LoadMUS("asset/music/06_Battle_in_Space_Intro.ogg");
    if(bgm==nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);
}

void SceneTitle::update(float deltaTime)
{
    timer += deltaTime;
    if(timer > 1.5f)
    {
        timer -= 1.5f;
    }
}

void SceneTitle::render()
{
    //渲染标题文字
    std::string title = "SPACESHIP FIGHT";
    game.renderTextCentered(title, 0.3f,true);
    //渲染普通文字
    if(timer < 1.0f)
    {
    std::string text = "Press Enter to Start";
    game.renderTextCentered(text, 0.8f,false);    
    }
    
}

void SceneTitle::clean()
{
    Mix_HaltMusic();
    Mix_FreeMusic(bgm);
}

void SceneTitle::handleEvent(SDL_Event *event)
{
    if(event->type == SDL_KEYDOWN)
    {
        if(event->key.keysym.sym == SDLK_RETURN)
        {
            auto sceneMain = new SceneMain();
            
            game.changeScene(sceneMain);
        }
    }
}
