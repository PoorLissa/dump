// Класс, в который завернута вся логика игры

#pragma once
#ifndef _GAME_H_
#define _GAME_H_

#include "__d3dClass.h"
#include "__bitmapClass.h"
#include "__SpriteInstanced.h"
#include "__directInput.h"
#include "__graphicsClass.h"

#include "gameShader_Bullet.h"
#include "gameCells.h"

#include "Bonus.h"
#include "BonusWeapons.h"
#include "Weapon.h"
#include "Player.h"

#define doLogMessages

// ------------------------------------------------------------------------------------------------------------------------



// Вспомогательные структуры для хранения списков игровых объектов и всей сопутствующей инфы:

// Монстры
struct MonsterList {
    std::list<gameObjectBase*> objList;
    unsigned int         listSize;
    unsigned int         framesQty;
    float                rotationAngle;
    InstancedSprite     *spriteInst;

    // добавляем нового монстра в список
    void pushBack(ciRef x, ciRef y, cfRef scale, cfRef speed, ciRef interval) {
        objList.push_back(new Monster(x, y, scale, rotationAngle, speed, interval, framesQty));
        listSize++;
    }
};

// Бонусы
struct BonusList {
    std::list<gameObjectBase*> objList;
    unsigned int         listSize;
    float                rotationAngle;
    InstancedSprite     *spriteInst;
};

// Оружие
struct WeaponList {
    std::list<gameObjectBase*> objList;
    unsigned int         listSize;
    InstancedSprite     *spriteInst;
};

// Пули
struct BulletList {
    std::list<gameObjectBase*> objList;
    unsigned int        listSize;
};
// ------------------------------------------------------------------------------------------------------------------------



// Предварительное объявление класса
class GraphicsClass;



class Game {

 public:
     Game();
    ~Game();

    bool Init(int, int, HighPrecisionTimer *, GraphicsClass *, HWND);
    void Shutdown();

    inline char* getMsg() { return msg; }

    bool Render2d(const float &, const float &, const int &, const int &, const keysPressed *, const bool &);

 private:
    void threadMonsterMove(const unsigned int &, const unsigned int &, const unsigned int &);

 private:

    // текстовое сообщение, которое можно вывести на экран
    char                    *msg;
    char                     chBuffer[100];

    // Экранные размеры
    int                      scrWidth, scrHeight,           // размеры видимого окна
                             scrHalfWidth, scrHalfHeight,   // половинные размеры видимого окна
                             wndPosX, wndPosY,              // смещение видимого окна относительно нулевой координаты
                             bgrWidth, bgrHeight;           // размеры всего игрового поля (в норме должны превышать размеры видимого окна)
    int                      borderZone;                    // ширина зазора вдоль границ экрана, на которой начинается сдвиг экрана
    unsigned int             qtyToGenerate;
    float                    timeElapsed;

    // Указатель на GraphicsClass, в котором класс Game объявлен как дружественный.
    // Таким образом мы сможем из класса игры рендерить объекты, не передавая в него миллион параметров
    GraphicsClass           *m_Graphics;

    HighPrecisionTimer       gameTimer;

    bulletShader_Instancing *m_BulletShader;

	BitmapClass			    *m_Bitmap_Bgr;
    BitmapClass			    *m_Cursor;

    InstancedSprite         *m_PlayerBitmapIns1;
    InstancedSprite         *m_PlayerBitmapIns2;
    InstancedSprite         *m_BulletBitmapIns;

    // Инстанцированные спрайты с анимацией
    InstancedSprite         *sprIns1;
    InstancedSprite         *sprIns2;
    InstancedSprite         *sprIns3;
    InstancedSprite         *sprIns4;

    // Списки игровых объектов
    MonsterList              monsterList1, monsterList2;
    BonusList                bonusList1;
    WeaponList               weaponList1;
    BulletList               bulletList;

    gameObjectBase          *m_Player;

    // Вектор, в котором содержатся все наши списки монстров. Передаем его в обработчик перемещения каждой пули для просчета стрельбы
    std::vector< MonsterList* > VEC;

    // указатель на пул потоков
    ThreadPool *thPool;
};

#endif
