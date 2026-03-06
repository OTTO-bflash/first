#ifndef SCENE_MAIN_H
#define SCENE_MAIN_H

#include "Scene.h"
#include "Object.h"
#include <list>
#include <random>
#include <SDL.h>
#include <SDL_mixer.h>
#include <map>
#include <SDL_ttf.h>
class Game;
class SceneMain : public Scene {
public:
    ~SceneMain();
    
    
    void handleEvent(SDL_Event* event) override;
    void init() override;
    void clean() override;
    void update(float deltaTime) override;
    void render() override;
 
private:
  
Player player;
Mix_Music* bgm;
SDL_Texture* uiHealth;
TTF_Font* scoreFont;
int score = 0;
float timerEnd=0.0f;

bool isDead=false;
std::mt19937 gen;
std::uniform_real_distribution<float> dis;
//创建每个物体的模板
Enemy enemyTemplate;
ProjectilePlayer projectilePlayerTemplate;
ProjectileEnemy projectileEnemyTemplate;
Explosion explosionTemplate;
Item itemLifeTemplate;
Item itemSpreadTemplate;
Item itemShieldTemplate;
//创建每个物体的容器
std::list<Enemy*> enemies; 
std::list<ProjectilePlayer*>   projectilesPlayer;
std::list<ProjectileEnemy*>    projectilesEnemy;
std::list<Explosion*>   explosions;
std::list<Item*> items;  //不同道具都能放
std::map<std::string, Mix_Chunk*> sounds; //存放音效
//渲染相关
void renderEnemies();
void renderItems();
void renderUI();
void renderExplosions();
void renderProjectiles();
void renderEnemyProjectiles();
void renderShield();
//更新相关

void updateEnemies(float deltaTime);
void updateEnemyProjectiles(float deltaTime);
void updatePlayer(float deltaTime);

void updateItems(float deltaTime);
void updatePlayerProjectiles(float deltaTime);
void updateExplosions(float deltaTime);
void spawnEnemies();
void keyboardControl(float deltaTime);
void changeSceneDelayed(float delatTime,float delay);
//其它
void playerGetItem(Item*item);
void shootPlayer();
void shootSpread();
void shootEnemy(Enemy* enemy); //敌人发射子弹
SDL_FPoint getDirection(Enemy*enemy);
void enemyExplode(Enemy* enemy);
void dropItem(Enemy*enemy);

};
#endif 