#ifndef SCENE_END_H
#define SCENE_END_H
#include "SDL_mixer.h"
#include "Scene.h"
#include <string>
class SceneEnd:public Scene
{
    public:
    virtual void init();
    virtual void update(float deltaTime);
    virtual void render();
    virtual void clean();
    virtual void handleEvent(SDL_Event* event);

private:
    bool isTyping=true;
    std::string name = "";
    float blinkTime = 1.0f;
    Mix_Music* bgm;
    void renderPhase1();
    void renderPhase2();
    
};


#endif
