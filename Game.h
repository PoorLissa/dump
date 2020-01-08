// �����, � ������� ��������� ��� ������ ����

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



// ��������������� ��������� ��� �������� ������� ������� �������� � ���� ������������� ����:

// �������
struct MonsterList {
    std::list<gameObjectBase*> objList;
    unsigned int         listSize;
    unsigned int         framesQty;
    float                rotationAngle;
    InstancedSprite     *spriteInst;

    // ��������� ������ ������� � ������
    void pushBack(ciRef x, ciRef y, cfRef scale, cfRef speed, ciRef interval) {
        objList.push_back(new Monster(x, y, scale, rotationAngle, speed, interval, framesQty));
        listSize++;
    }
};

// ������
struct BonusList {
    std::list<gameObjectBase*> objList;
    unsigned int         listSize;
    float                rotationAngle;
    InstancedSprite     *spriteInst;
};

// ������
struct WeaponList {
    std::list<gameObjectBase*> objList;
    unsigned int         listSize;
    InstancedSprite     *spriteInst;
};

// ����
struct BulletList {
    std::list<gameObjectBase*> objList;
    unsigned int        listSize;
};
// ------------------------------------------------------------------------------------------------------------------------



// ��������������� ���������� ������
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

    // ��������� ���������, ������� ����� ������� �� �����
    char                    *msg;
    char                     chBuffer[100];

    // �������� �������
    int                      scrWidth, scrHeight,           // ������� �������� ����
                             scrHalfWidth, scrHalfHeight,   // ���������� ������� �������� ����
                             wndPosX, wndPosY,              // �������� �������� ���� ������������ ������� ����������
                             bgrWidth, bgrHeight;           // ������� ����� �������� ���� (� ����� ������ ��������� ������� �������� ����)
    int                      borderZone;                    // ������ ������ ����� ������ ������, �� ������� ���������� ����� ������
    unsigned int             qtyToGenerate;
    float                    timeElapsed;

    // ��������� �� GraphicsClass, � ������� ����� Game �������� ��� �������������.
    // ����� ������� �� ������ �� ������ ���� ��������� �������, �� ��������� � ���� ������� ����������
    GraphicsClass           *m_Graphics;

    HighPrecisionTimer       gameTimer;

    bulletShader_Instancing *m_BulletShader;

	BitmapClass			    *m_Bitmap_Bgr;
    BitmapClass			    *m_Cursor;

    InstancedSprite         *m_PlayerBitmapIns1;
    InstancedSprite         *m_PlayerBitmapIns2;
    InstancedSprite         *m_BulletBitmapIns;

    // ���������������� ������� � ���������
    InstancedSprite         *sprIns1;
    InstancedSprite         *sprIns2;
    InstancedSprite         *sprIns3;
    InstancedSprite         *sprIns4;

    // ������ ������� ��������
    MonsterList              monsterList1, monsterList2;
    BonusList                bonusList1;
    WeaponList               weaponList1;
    BulletList               bulletList;

    gameObjectBase          *m_Player;

    // ������, � ������� ���������� ��� ���� ������ ��������. �������� ��� � ���������� ����������� ������ ���� ��� �������� ��������
    std::vector< MonsterList* > VEC;

    // ��������� �� ��� �������
    ThreadPool *thPool;
};

#endif
