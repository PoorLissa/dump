#pragma once
#ifndef _BULLET_H_
#define _BULLET_H_

#include "gameObjectBase.h"

// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ����
class Bullet : public gameObjectBase {

 public:
    Bullet(cfRef, cfRef, cfRef, cfRef, cfRef, cfRef, UINT);
   ~Bullet() {}

    // ����� ��� ��������� �������� _scrWidth � _scrHeight, �� ��������� ������� ���� ����� ��������
    static void setScrSize(cuiRef width, cuiRef height) {
        unsigned int addedSize = 100;
        _scrWidth  = width  + addedSize;
        _scrHeight = height + addedSize;
    }

    virtual inline cuiRef getAnimPhase() const { return   0; }
            inline cfRef  getX0()        const { return _X0; }
            inline cfRef  getY0()        const { return _Y0; }

    // ������������ �������� ����, ������������ �� � �������� ��� ����� ����������
    inline virtual void Move(cfRef wndPosX, cfRef wndPosY, void *Param) { _thPool->runAsync(&Bullet::threadMove_Cells, this, wndPosX, wndPosY); }

    inline       void          setBulletType(const unsigned int &type)  { _bulletType = type; }
    inline const unsigned int& getBulletType()                          { return _bulletType; }
    inline       void          setPiercing(const bool &mode)            {   _piercing = mode; }
    inline const bool        & isPiercing()                             {   return _piercing; }

 protected:
    // ����������� ������� � �����������
    // http://www.cyberforum.ru/cpp-beginners/thread853799.html
    bool commonSectionCircle(float, float, float, float, const int &, const int &, const float &);

 private:
    // ��������� ������� ��������� ���� � ��������
    void threadMove_VECT(void *);           // ������ �������, ��� �� ������������
    void threadMove_Cells(cfRef, cfRef);    // ������ ������� � ��������, �������� �� ��������

 protected:
    float      _X0, _Y0;        // ����������� �����, �� ������� ���� �����
    float      _dX, _dY;        // �������� �� x � �� y ��� ���������� ����� ������� ����
    float      _dX0, _dY0;      // �������� �� x � �� y ��� ������ ����, ����� �� ���� �������� �� ������

    static int _scrWidth;       // �������� ���������, �� ��������� ������� ���� ��������� ������� � ������
    static int _scrHeight;      // �������� ���������, �� ��������� ������� ���� ��������� ������� � ������

    int        _squareSide;

    UINT       _bulletType;     // 0 - ������� ����, 1 - ��������, 2 - ������, 3 - ����������, 4 - ����������, 5 - ��������������
    bool       _piercing;       // false - ���� ��� ��������� � ������� ��������, true - ���� ����������� �������, ����� ����� ����� ����� (true ��� �������� ����� � ��������� ������)
    bool       _Shot, _bX, _bY; // ����� ��� ��������� ������ ���� ��� ��������� � �������
};

#endif