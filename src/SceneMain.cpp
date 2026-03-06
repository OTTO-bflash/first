#include "SceneMain.h"
#include "Game.h"
#include "SceneTitle.h"
#include <SDL.h>
#include <SDL_image.h>
#include <random>
#include <string>
#include "SceneEnd.h"
SceneMain::~SceneMain()
{
}

void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);
    updatePlayerProjectiles(deltaTime);
    updateEnemyProjectiles(deltaTime);
    spawnEnemies(); 
    updatePlayer(deltaTime);
    
    updateEnemies(deltaTime);
    updateExplosions(deltaTime);
    updateItems(deltaTime);
    
    if(isDead){
        changeSceneDelayed(deltaTime,2);
    }
}

void SceneMain::render()
{
    //渲染子弹
    renderProjectiles();
    //渲染敌机子弹
    renderEnemyProjectiles();
    //渲染玩家
    if(!isDead){
        SDL_Rect playerRect = { static_cast<int>(player.position.x),
                            static_cast<int>(player.position.y),
                            player.width,
                            player.height };
    SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);
    }
    //渲染护盾
    
    renderShield();
   
    //渲染敌人
    renderEnemies();
    //渲染道具
    renderItems();
    //渲染爆炸
    renderExplosions();
    //渲染ui
    renderUI();

}
void SceneMain::handleEvent(SDL_Event* event)
{
    if(event->type == SDL_KEYDOWN)
    {
        if(event->key.keysym.sym == SDLK_ESCAPE)
        {
            auto sceneTitle = new SceneTitle();
            game.changeScene(sceneTitle);
        }
    }

}
void SceneMain::init()
{
    //读取并播放背景音乐
    bgm=Mix_LoadMUS("asset/music/Sketchbook 2025-12-14 LOOP.ogg");
    if(bgm == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Failed to load music: %s",SDL_GetError());
    }
    Mix_PlayMusic(bgm,-1);
    //读取ui Health
    uiHealth=IMG_LoadTexture(game.getRenderer(), "asset/image/Health UI Black.png");
    //载入字体
    scoreFont=TTF_OpenFont("asset/font/VonwaonBitmap-12px.ttf", 24);
    
    //读取音效资源
    sounds["player_shoot"]=Mix_LoadWAV("asset/sound/laser/laser_shoot4.mp3"); //音乐是MUS，音效是WAV
    sounds["enemy_shoot"]=Mix_LoadWAV("asset/sound/laser/xs_laser.mp3");
    sounds["player_explosion"]=Mix_LoadWAV("asset/sound/explosion/explosion1.mp3");
    sounds["enemy_explosion"]=Mix_LoadWAV("asset/sound/explosion/explosion4.mp3");
    sounds["hit"]=Mix_LoadWAV("asset/sound/utility/eff11.mp3");
    sounds["get_item"]=Mix_LoadWAV("asset/sound/utility/eff5.mp3");





    std::random_device rd;
    gen = std::mt19937(rd());
    dis = std::uniform_real_distribution<float> (0.0f,1.0f);
    


    player.texture=IMG_LoadTexture(game.getRenderer(), "asset/image/SpaceShip.png");
    if(player.texture == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Failed to load texture: %s",SDL_GetError());
    }
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
    player.width/=3;
    player.height/=3;
    player.position.x = static_cast<float>(game.getWindowWidth() / 2 - player.width / 2);
    player.position.y = static_cast<float>(game.getWindowHeight() - player.height);

    player.Shieldtexture=IMG_LoadTexture(game.getRenderer(), "asset/image/shield.png");
    SDL_QueryTexture(player.Shieldtexture, NULL, NULL, &player.Shieldwidth, &player.Shieldheight);
    player.Shieldwidth/=3;
    player.Shieldheight/=3;
    //初始化模板
    projectilePlayerTemplate.texture=IMG_LoadTexture(game.getRenderer(), "asset/image/laser-2.png");
    SDL_QueryTexture(projectilePlayerTemplate.texture, NULL, NULL, &projectilePlayerTemplate.width, &projectilePlayerTemplate.height);
    projectilePlayerTemplate.width/=3;
    projectilePlayerTemplate.height/=3;

    enemyTemplate.texture=IMG_LoadTexture(game.getRenderer(), "asset/image/insect-1.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width/=3;
    enemyTemplate.height/=3;

    projectileEnemyTemplate.texture=IMG_LoadTexture(game.getRenderer(), "asset/image/bullet-1.png");
    SDL_QueryTexture(projectileEnemyTemplate.texture, NULL, NULL, &projectileEnemyTemplate.width, &projectileEnemyTemplate.height);
    projectileEnemyTemplate.width/=3;
    projectileEnemyTemplate.height/=3;

    explosionTemplate.texture=IMG_LoadTexture(game.getRenderer(), "asset/effect/spritesheet.png");
    SDL_QueryTexture(explosionTemplate.texture, NULL, NULL, &explosionTemplate.width, &explosionTemplate.height);
    explosionTemplate.totalFrame=explosionTemplate.width/explosionTemplate.height;
    explosionTemplate.width=explosionTemplate.height;

    itemLifeTemplate.texture=IMG_LoadTexture(game.getRenderer(), "asset/image/bonus_life.png");
    SDL_QueryTexture(itemLifeTemplate.texture, NULL, NULL, &itemLifeTemplate.width, &itemLifeTemplate.height);
    itemLifeTemplate.type=ItemType::Health;
    itemLifeTemplate.width/=4;
    itemLifeTemplate.height/=4;

    itemSpreadTemplate.texture=IMG_LoadTexture(game.getRenderer(), "asset/image/plasm.png");
    SDL_QueryTexture(itemSpreadTemplate.texture, NULL, NULL, &itemSpreadTemplate.width, &itemSpreadTemplate.height);
    itemSpreadTemplate.type=ItemType::Spread;
    itemSpreadTemplate.width/=3;
    itemSpreadTemplate.height/=3;

    itemShieldTemplate.texture=IMG_LoadTexture(game.getRenderer(), "asset/image/laser-3.png");
    SDL_QueryTexture(itemShieldTemplate.texture, NULL, NULL, &itemShieldTemplate.width, &itemShieldTemplate.height);
    itemShieldTemplate.type=ItemType::Shield;
    itemShieldTemplate.width/=3;
    itemShieldTemplate.height/=3;
}

void SceneMain::clean()
{
    //清理容器
    for(auto sound:sounds){
        if(sound.second != nullptr){
            Mix_FreeChunk(sound.second);
        }
    }
    sounds.clear();
    
    for(auto &projectile : projectilesPlayer)
    {
        if (projectile != nullptr)
        {
            delete projectile;
        }
    }
    projectilesPlayer.clear();
    for(auto &enemy : enemies)
    {
        if(enemy != nullptr)
        {
            delete enemy;
        }
    }
    enemies.clear();
    for(auto &projectile : projectilesEnemy)
    {
        if (projectile != nullptr)
        {
            delete projectile;
        }
    }
    projectilesEnemy.clear();
    for(auto&item:items){
        if(item != nullptr)
        {
            delete item;
        }
    }
    items.clear();
    //清理ui
   if(uiHealth != nullptr){
    SDL_DestroyTexture(uiHealth);
   }

   if(scoreFont != nullptr){
       TTF_CloseFont(scoreFont);
   }
    //清理模板
   if(player.texture != nullptr)
   {
      SDL_DestroyTexture(player.texture);
   }
   if(player.Shieldtexture != nullptr)
   {
       SDL_DestroyTexture(player.Shieldtexture);
   }
   if(projectilePlayerTemplate.texture != nullptr)
   {
       SDL_DestroyTexture(projectilePlayerTemplate.texture);
   }
   if(enemyTemplate.texture != nullptr)
   {
       SDL_DestroyTexture(enemyTemplate.texture);
   }
   if(projectileEnemyTemplate.texture != nullptr)
   {
       SDL_DestroyTexture(projectileEnemyTemplate.texture);
   }
   if(explosionTemplate.texture != nullptr)
   {
       SDL_DestroyTexture(explosionTemplate.texture);
   }
   if(itemLifeTemplate.texture != nullptr)
   {
       SDL_DestroyTexture(itemLifeTemplate.texture);
   }
   if(itemSpreadTemplate.texture != nullptr)
   {
       SDL_DestroyTexture(itemSpreadTemplate.texture);
   }
   if(itemShieldTemplate.texture != nullptr)
   {
       SDL_DestroyTexture(itemShieldTemplate.texture);
   }
   //清理音乐
   if(bgm != nullptr)
   {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
   }
}



void SceneMain::keyboardControl(float deltaTime)
{
    if(isDead){
        return;
    }
    auto keyboardState=SDL_GetKeyboardState(NULL);
    if(keyboardState[SDL_SCANCODE_W])
    {
        player.position.y -= deltaTime*player.speed;
    }
    if(keyboardState[SDL_SCANCODE_S])
    {
        player.position.y += deltaTime*player.speed;
    }
    if(keyboardState[SDL_SCANCODE_A])
    {
        player.position.x -= deltaTime*player.speed;
    }
    if(keyboardState[SDL_SCANCODE_D])
    {
        player.position.x += deltaTime*player.speed;
    }
    //限制移动范围
    if(player.position.x < 0)
    {
        player.position.x = 0;
    }
    if(player.position.x > game.getWindowWidth() - player.width)
    {
        player.position.x = static_cast<float>(game.getWindowWidth() - player.width);
    }
    if(player.position.y < 0)
    {
        player.position.y = 0;
    }
    if(player.position.y > game.getWindowHeight() - player.height)
    {
        player.position.y = static_cast<float>(game.getWindowHeight() - player.height);
    }
    //控制子弹发射
    // if(keyboardState[SDL_SCANCODE_J])
    // {
    //    auto currenTime =SDL_GetTicks(); //发射子弹
    //    if(currenTime - player.lastShootTime > player.coolDown)
    //    {
    //         shootPlayer();
    //        player.lastShootTime = currenTime;
    // }
    // }   
}

void SceneMain::changeSceneDelayed(float deltaTime, float delay)
{
    timerEnd +=deltaTime;
    if(timerEnd >= delay)
    {
        auto sceneEnd =new SceneEnd();
        game.changeScene(sceneEnd);
    }
}

void SceneMain::shootPlayer()
{
    auto projectile= new ProjectilePlayer(projectilePlayerTemplate);
    projectile->position.x = player.position.x + player.width/2 - projectile->width/2;
    projectile->position.y = player.position.y;
    projectile->direction={0.0f,-1.0f};
    projectile->angle=0.0f;
    projectilesPlayer.push_back(projectile);
    Mix_PlayChannel(0, sounds["player_shoot"], 0);
}

void SceneMain::shootSpread()
{
    float xSpeed[]={-0.3f,-0.1f,0.0f,0.1f,0.3f};
    float playerCenterX = player.position.x + player.width / 2.0f; // 计算玩家中心的x坐标
    for(auto vx: xSpeed){
        auto projectile = new ProjectilePlayer(projectilePlayerTemplate);
        float offset= vx*60.0f; // 计算子弹的偏移量
        float bulletCenterX = playerCenterX + offset; // 计算子弹中心的x坐标
        float bulletX = bulletCenterX - projectile->width / 2.0f; // 计算子弹的x坐标
        projectile->position.x = bulletX;
        projectile->position.y = player.position.y;
        projectile->direction.x=vx;
        projectile->direction.y=-1.0f;
        projectile->angle=atan2(projectile->direction.y,projectile->direction.x)*180.0f/M_PI+90;
        projectile->speed=static_cast<float>(projectile->speed*1.5f);
        projectilesPlayer.push_back(projectile);
        Mix_PlayChannel(0, sounds["player_shoot"], 0);
    }
}

void SceneMain::updatePlayerProjectiles(float deltaTime)
{
    int margin=32;
    for(auto it = projectilesPlayer.begin(); it != projectilesPlayer.end();)
    {
        auto projectile = *it;
        projectile->position.y -= projectile->speed * deltaTime;
        projectile->position.x += projectile->direction.x * projectile->speed * deltaTime;
        //检查子弹是否超出屏幕
        if(projectile->position.y < -margin)
        {
            delete projectile;
            it=projectilesPlayer.erase(it);
            
        }
        else
        {
            bool hit =false;
            for(auto enemy : enemies)
            {
                SDL_Rect enemyRect={
                    static_cast<int>(enemy->position.x),
                    static_cast<int>(enemy->position.y),
                    enemy->width,
                    enemy->height
                };
                SDL_Rect projectileRect = { 
                    static_cast<int>(projectile->position.x),
                    static_cast<int>(projectile->position.y),
                    projectile->width,
                    projectile->height
                };
                if (SDL_HasIntersection(&enemyRect, &projectileRect)) {
                    enemy->currentHealth -= projectile->damage;
                    delete projectile;
                    it = projectilesPlayer.erase(it);
                    hit = true;
                    Mix_PlayChannel(-1, sounds["hit"], 0);
                    
                    break;
                }
            }
            if(!hit)
            {
                ++it;
            }
          
        }
    }
}
void SceneMain::renderProjectiles()
{
    for(auto projectile:projectilesPlayer)
    {
        SDL_Rect projectileRect = { 
        static_cast<int>(projectile->position.x),
        static_cast<int>(projectile->position.y),
        projectile->width,
        projectile->height
    };
    SDL_RenderCopyEx(game.getRenderer(), projectile->texture, NULL, &projectileRect,
    projectile->angle, NULL, SDL_FLIP_NONE);
    }
   
}



void SceneMain::renderEnemyProjectiles()
{
    for(auto projectile:projectilesEnemy)
    {
        SDL_Rect projectileRect = { 
        static_cast<int>(projectile->position.x),
        static_cast<int>(projectile->position.y),
        projectile->width,
        projectile->height
    };
    //SDL_RenderCopy(game.getRenderer(), projectile->texture, NULL, &projectileRect);
    float angel= static_cast<float>(atan2(projectile->direction.y,projectile->direction.x)*180/M_PI-90);
    SDL_RenderCopyEx(game.getRenderer(), projectile->texture, NULL, &projectileRect, angel, NULL, SDL_FLIP_NONE);
    }
}

void SceneMain::renderShield()
{
    if(!player.hasShield || isDead){
        return;
    }
    SDL_Rect shieldRect = {
    static_cast<int>(player.position.x+player.width/2-player.Shieldwidth/2),
    static_cast<int>(player.position.y-30),
    player.Shieldwidth,
    player.Shieldheight
    };
    SDL_RenderCopy(game.getRenderer(), player.Shieldtexture, NULL, &shieldRect);
}

void SceneMain::spawnEnemies()
{
    if(dis(gen)>1/60.0f)
    {
        return;
    }
    Enemy* enemy = new Enemy(enemyTemplate);
    enemy->position.x = dis(gen) * (game.getWindowWidth() - enemy->width);
    enemy->position.y = -static_cast<float>(enemy->height);
    enemies.push_back(enemy);
}

void SceneMain::updateEnemies(float deltaTime)
{
    auto currenTime = SDL_GetTicks();
    for(auto it = enemies.begin(); it != enemies.end();)
    {
        auto enemy = *it;
        enemy->position.y += enemy->speed * deltaTime;
        if(enemy->position.y > game.getWindowHeight())
        {
            delete enemy;
            it = enemies.erase(it);
        }
        else
        {
            if(currenTime-enemy->lastShootTime> enemy->coolDown&&!isDead)
            {
                shootEnemy(enemy);
                enemy->lastShootTime = currenTime;
            }
            SDL_Rect EnemyRect={
                    static_cast<int>(enemy->position.x),
                    static_cast<int>(enemy->position.y),
                    enemy->width,
                    enemy->height
            };
                SDL_Rect shieldRect = {
                static_cast<int>(player.position.x+player.width/2-player.Shieldwidth/2),
                static_cast<int>(player.position.y-30),
                player.Shieldwidth,
                player.Shieldheight
                };
            if(enemy->currentHealth <= 0 )
            {
                enemyExplode(enemy);
                it = enemies.erase(it);
            } else if(player.hasShield&&SDL_HasIntersection(&EnemyRect, &shieldRect))
            {
                enemyExplode(enemy);
                it = enemies.erase(it);
            }
            else{
                ++it;
            }
        }
    }
}

void SceneMain::updateEnemyProjectiles(float deltaTime)
{
    auto margin=32;
    for(auto it = projectilesEnemy.begin(); it != projectilesEnemy.end();)
    {
        auto projectile = *it;
        projectile->position.x += projectile->speed*projectile->direction.x*deltaTime;
        projectile->position.y += projectile->speed*projectile->direction.y*deltaTime;
        if(projectile->position.y > game.getWindowHeight()+margin||
        projectile->position.y < -margin||
        projectile->position.x <- margin||
        projectile->position.x > game.getWindowWidth()+margin)
        {
            delete projectile;
            it = projectilesEnemy.erase(it);
        }
       
        else
        
        {
             
                SDL_Rect playerRect={
                    static_cast<int>(player.position.x),
                    static_cast<int>(player.position.y),
                    player.width,
                    player.height
                };
                SDL_Rect shieldRect = {
                static_cast<int>(player.position.x+player.width/2-player.Shieldwidth/2),
                static_cast<int>(player.position.y-30),
                player.Shieldwidth,
                player.Shieldheight
                };
                
                SDL_Rect projectileRect = { 
                    static_cast<int>(projectile->position.x),
                    static_cast<int>(projectile->position.y),
                    projectile->width,
                    projectile->height
                };
                
                if(SDL_HasIntersection(&shieldRect, &projectileRect)&&!isDead){
                    delete projectile;
                    it = projectilesEnemy.erase(it);
                    Mix_PlayChannel(-1, sounds["hit"], 0);
                }else if(SDL_HasIntersection(&playerRect, &projectileRect)&&!isDead)
                {
                    player.currentHealth -= projectile->damage;
                    delete projectile;
                    it = projectilesEnemy.erase(it);
                    Mix_PlayChannel(-1, sounds["hit"], 0);
                }
                else{
                    ++it;
                }
            
        }
    
    }
}

void SceneMain::updatePlayer(float deltaTime)
{
    if(isDead){
        return;
    }
    Uint32 currentCoolDown = player.coolDown;
    if(player.isSpreadshot){
        currentCoolDown/=2;
        player.spreadTimer-=deltaTime;
        if(player.spreadTimer<=0){
            player.isSpreadshot=false;
            SDL_Log("Power-up Expired: Back to normal.");
        }
    }
    if(player.hasShield)
    {
        player.shieldTimer-=deltaTime;
        if(player.shieldTimer<=0){
            player.hasShield=false;
        }
    }
auto currenTime = SDL_GetTicks(); 
// 只要当前时间减去上次发射时间超过了冷却时间，就发射
if (currenTime - player.lastShootTime > player.coolDown)
{
    if(player.isSpreadshot){
        shootSpread();  // 执行发射动作
    }else{
        shootPlayer(); // 执行发射动作
    }
    
    player.lastShootTime = currenTime; // 重置计时器

}
for(auto enemy:enemies){
    
    SDL_Rect playerRect={
        static_cast<int>(player.position.x),
        static_cast<int>(player.position.y),
        player.width,
        player.height
    };
    SDL_Rect enemyRect = { 
        static_cast<int>(enemy->position.x),
        static_cast<int>(enemy->position.y),
        enemy->width,
        enemy->height
    };
    if (SDL_HasIntersection(&playerRect, &enemyRect)&&!isDead) {
        player.currentHealth -= 1;
        enemy->currentHealth = 0;;
        
    }
}
                


    if(player.currentHealth <= 0){
        isDead=true;
        auto explosion= new Explosion(explosionTemplate);
        explosion->position.x = player.position.x+ player.width/2-explosion->width/2;
        explosion->position.y = player.position.y+player.height/2-explosion->height/2;
        explosion->startTime = currenTime;
        explosions.push_back(explosion);
        Mix_PlayChannel(-1, sounds["player_explosion"], 0);
        game.setFinalScore(score);
        return;
    }
}



void SceneMain::renderEnemies()
{
    for(auto enemy : enemies)
    {
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height
        };
        SDL_RenderCopy(game.getRenderer(), enemy->texture, NULL, &enemyRect);
    }
}

void SceneMain::shootEnemy(Enemy *enemy)
{
    auto projectile = new ProjectileEnemy(projectileEnemyTemplate);
    projectile->position.x = enemy->position.x + enemy->width/2 - projectile->width/2;
    projectile->position.y = enemy->position.y + enemy->height;
    projectile->direction=getDirection(enemy);
    projectilesEnemy.push_back(projectile);
    Mix_PlayChannel(-1, sounds["enemy_shoot"], 0);
}

SDL_FPoint SceneMain::getDirection(Enemy *enemy)
{
    auto x=(player.position.x + player.width/2) - (enemy->position.x + enemy->width/2);
    auto y=(player.position.y + player.height/2) - (enemy->position.y + enemy->height/2);
    auto length = sqrt(x*x + y*y);
    x/=length;
    y/=length;
    return SDL_FPoint{x,y};
}



void SceneMain::enemyExplode(Enemy *enemy)
{
    auto currenTime = SDL_GetTicks();
    auto explosion= new Explosion(explosionTemplate);
    explosion->position.x = enemy->position.x+ enemy->width/2-explosion->width/2;
    explosion->position.y = enemy->position.y+enemy->height/2-explosion->height/2;
    explosion->startTime = currenTime;
    explosions.push_back(explosion);
    Mix_PlayChannel(-1, sounds["enemy_explosion"], 0);
    if(dis(gen)<0.5f){
        dropItem(enemy);
    }
    score+=10;
    delete enemy;
}

void SceneMain::updateExplosions(float deltaTime)
{
    auto currentTime = SDL_GetTicks();
    for(auto it = explosions.begin(); it != explosions.end();)
    {
        auto explosion = *it;
        explosion->currentFrame=(currentTime-explosion->startTime)*explosion->FPS/1000;
        if(explosion->currentFrame >= explosion->totalFrame){
            delete explosion;
            it = explosions.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void SceneMain::renderExplosions()
{
    for(auto explosion:explosions)
    {
        SDL_Rect src = {
            explosion->currentFrame*explosion->width,
            0,
            explosion->width,
            explosion->height
        };
        SDL_Rect dstRect = {
            static_cast<int>(explosion->position.x),
            static_cast<int>(explosion->position.y),
            explosion->width,
            explosion->height
        };
        SDL_RenderCopy(game.getRenderer(), explosion->texture, &src, &dstRect);
    }
}

void SceneMain::dropItem(Enemy* enemy)
{
    Item*item = nullptr;
    float r =dis(gen);
    if(r<0.4f){
        item = new Item(itemLifeTemplate);
    }
    else if(r<0.7f){
        item = new Item(itemSpreadTemplate);
    }else{
        item =new Item(itemShieldTemplate);
    }
    
    //auto item = new Item(itemLifeTemplate);
    item->position.x = enemy->position.x + enemy->width/2 - item->width/2;
    item->position.y = enemy->position.y + enemy->height/2 - item->height/2;
    float angle=dis(gen) *2*M_PI;
    item->direction.x = cos(angle); //要求弧度
    item->direction.y = sin(angle);
    items.push_back(item); 
}

void SceneMain::updateItems(float deltaTime)
{
    for(auto it = items.begin(); it != items.end();)
    {
        auto item = *it;
        //更新位置
        item->position.x += item->direction.x * item->speed * deltaTime;
        item->position.y += item->direction.y * item->speed * deltaTime;
        //处理边缘反弹
        if( item->position.x < 0 &&item->bounceCount>0){
            item->direction.x =-item->direction.x;
            item->bounceCount--;
        }
        if(item->position.x +item->width>game.getWindowWidth()&&item->bounceCount>0){
            item->direction.x =-item->direction.x;
            item->bounceCount--;
        }
        if(item->position.y < 0&&item->bounceCount>0){
            item->direction.y =-item->direction.y;
            item->bounceCount--;
        }
        if(item->position.y +item->height>game.getWindowHeight()&&item->bounceCount>0){
            item->direction.y =-item->direction.y;
            item->bounceCount--;
        }
        //判断是否超出屏幕
        if(item->position.x +item->width<0 ||
            item->position.x>game.getWindowWidth() ||
            item->position.y +item->height<0 ||
            item->position.y>game.getWindowHeight()
            ){
            delete item;
            it = items.erase(it);
        }
        else 
        {
            SDL_Rect itemRect = {
                static_cast<int>(item->position.x),
                static_cast<int>(item->position.y),
                item->width,
                item->height
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height
            };
            if(SDL_HasIntersection(&itemRect, &playerRect)&& isDead==false){
                playerGetItem(item);
                delete item;
                it = items.erase(it);
            }
            else{
                ++it;
            }
        }
    }
              
    
}

void SceneMain::playerGetItem(Item *item)
{
    score+=5;
    if(item->type==ItemType::Health && player.currentHealth<player.maxHealth){
        player.currentHealth ++;
    }
    else if(item->type==ItemType::Spread ){
        player.isSpreadshot= true;
        player.spreadTimer=player.spreadDuration;

    }
    else if(item->type==ItemType::Shield){
        player.hasShield= true;
        player.shieldTimer=player.shieldDuration;
    }
    Mix_PlayChannel(-1, sounds["get_item"], 0);
}

void SceneMain::renderItems()
{
    for(auto item:items)
    {
        SDL_Rect itemRect = {
            static_cast<int>(item->position.x),
            static_cast<int>(item->position.y),
            item->width,
            item->height
        };
           
        
        SDL_RenderCopy(game.getRenderer(),item->texture,NULL,&itemRect);
    }
}

void SceneMain::renderUI()
{
    //血量
    int x=10;
    int y=10;
    int size=32;
    int offset=40;
    SDL_SetTextureColorMod(uiHealth, 100, 100, 100);
    for(int i=0;i<player.maxHealth;i++){
        SDL_Rect dstRect = {
            x+i*offset,
            y,
            size,
            size
        };
        SDL_RenderCopy(game.getRenderer(),uiHealth,NULL,&dstRect);
    }
     SDL_SetTextureColorMod(uiHealth, 255, 255, 255);
    for(int i=0;i<player.currentHealth;i++){
        SDL_Rect dstRect = {
            x+i*offset,
            y,
            size,
            size
        };
        SDL_RenderCopy(game.getRenderer(),uiHealth,NULL,&dstRect);
    }
    //分数
    auto text = "SCORE:" + std::to_string(score);
    SDL_Color color={255,255,255,255};
    SDL_Surface* surface = TTF_RenderUTF8_Solid(scoreFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game.getRenderer(), surface);
    SDL_Rect dstRect = {
        game.getWindowWidth()-10-surface->w,
        10,
        surface->w,
        surface->h
    };
    SDL_RenderCopy(game.getRenderer(),texture,NULL,&dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
