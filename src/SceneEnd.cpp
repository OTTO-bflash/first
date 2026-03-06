#include "SceneEnd.h"
#include"SceneMain.h"
#include "Game.h"
#include <string>
#include <SDL_mixer.h>
void SceneEnd::init()
{
    //载入背景音乐
    bgm =Mix_LoadMUS("asset/music/06_Battle_in_Space_Intro.ogg");
    if(!bgm){
        SDL_LogError(SDL_LOG_PRIORITY_ERROR,"Failed to load music: %s", Mix_GetError());
    }

    if(SDL_IsTextInputActive())
    {
        SDL_StartTextInput();
    }
    if(!SDL_IsTextInputActive()){
        SDL_LogError(SDL_LOG_PRIORITY_ERROR,"SDL","SDL_IsTextInputActive() failed: %s", SDL_GetError());
    }
   
}

void SceneEnd::update(float deltaTime)
{
    blinkTime -= deltaTime;
    if(blinkTime<=0.0f){
        blinkTime+=1.0f;
    }
}

void SceneEnd::render()
{
    if(isTyping){
        renderPhase1();
    }else{
        renderPhase2();
    }
}

void SceneEnd::clean()
{
    if(bgm!= nullptr){
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
}

void SceneEnd::handleEvent(SDL_Event *event)
{
    if(isTyping){
        if(event->type == SDL_TEXTINPUT){
            name += event->text.text;
           
        }
        if(event->type == SDL_KEYDOWN){
            
            if(event->key.keysym.scancode == SDL_SCANCODE_RETURN)
            {
                isTyping = false;
                SDL_StopTextInput();
                if(name =="")
                {
                    name ="ANON";
                }
                game.insertLeaderBoard(game.getFinalScore(),name);
            }
             if(event->key.keysym.scancode== SDL_SCANCODE_BACKSPACE){
                if (!name.empty()) {
                name.pop_back();
                }
            }
        }
    }
    else{
        if(event->type == SDL_KEYDOWN && event->key.keysym.scancode == SDL_SCANCODE_RETURN)
        {
            auto sceneMain = new SceneMain();
            game.changeScene(sceneMain);
        }
    }
}

void SceneEnd::renderPhase1()
{
    auto score = game.getFinalScore();
    std::string scoreText = "Your Score: " + std::to_string(score);
    std::string gameover = "Game Over" ;
    std::string instrutionText1 = "Print your name";
    std::string instrutionText2 = "And press Enter key to continue:";
    game.renderTextCentered(scoreText,0.2f,false);
    game.renderTextCentered(gameover,0.4f,true);
    game.renderTextCentered(instrutionText1,0.6f,false);
    game.renderTextCentered(instrutionText2,0.7f,false);

    if(name!=""){
        SDL_Point p=game.renderTextCentered(name,0.8f,false);
        if(blinkTime<0.5f){
            game.renderTextPos("_",p.x,p.y);
        }
       
    }else{
        if(blinkTime<0.5f){
            game.renderTextCentered("_",0.8f,false);
        }
    }

}

void SceneEnd::renderPhase2()
{
    game.renderTextCentered("SCOREBOARD",0.1,true);
    auto posY =static_cast<float>(0.2*game.getWindowHeight());
    int i =1;
    for(auto item :game.getLeaderBoard())
    {   
        
        std::string name = std::to_string(i)+". "+item.second;
        std::string score=std::to_string(item.first);
        game.renderTextPos(name,100,posY);
        game.renderTextPos(score,100,posY,false);
        posY+=45;
        i++;
    }
    if(blinkTime<0.5f)
    {
        game.renderTextCentered("Press Enter to go to Menu",0.85f,true);
    }
    
}
