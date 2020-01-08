#pragma once
#ifndef _BULLET_H_
#define _BULLET_H_

#include "gameObjectBase.h"

// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Пуля
class Bullet : public gameObjectBase {

 public:
    Bullet(cfRef, cfRef, cfRef, cfRef, cfRef, cfRef, UINT);
   ~Bullet() {}

    // Метод для установки значений _scrWidth и _scrHeight, за пределами которых пули будут исчезать
    static void setScrSize(cuiRef width, cuiRef height) {
        unsigned int addedSize = 100;
        _scrWidth  = width  + addedSize;
        _scrHeight = height + addedSize;
    }

    virtual inline cuiRef getAnimPhase() const { return   0; }
            inline cfRef  getX0()        const { return _X0; }
            inline cfRef  getY0()        const { return _Y0; }

    // просчитываем движение пули, столкновение ее с монстром или конец траектории
    inline virtual void Move(cfRef wndPosX, cfRef wndPosY, void *Param) { _thPool->runAsync(&Bullet::threadMove_Cells, this, wndPosX, wndPosY); }

    inline       void          setBulletType(const unsigned int &type)  { _bulletType = type; }
    inline const unsigned int& getBulletType()                          { return _bulletType; }
    inline       void          setPiercing(const bool &mode)            {   _piercing = mode; }
    inline const bool        & isPiercing()                             {   return _piercing; }

 protected:
    // пересечение отрезка с окружностью
    // http://www.cyberforum.ru/cpp-beginners/thread853799.html
    bool commonSectionCircle(float, float, float, float, const int &, const int &, const float &);

 private:
    // Потоковый просчет попаданий пули в монстров
    void threadMove_VECT(void *);           // первый вариант, уже не используется
    void threadMove_Cells(cfRef, cfRef);    // второй вариант с ячейками, зверский по скорости

 protected:
    float      _X0, _Y0;        // изначальная точка, из которой пуля летит
    float      _dX, _dY;        // смещения по x и по y для нахождения новой позиции пули
    float      _dX0, _dY0;      // смещения по x и по y для хвоста пули, чтобы он тоже удалялся от игрока

    static int _scrWidth;       // Значения координат, за пределами которых пуля считается ушедшей в молоко
    static int _scrHeight;      // Значения координат, за пределами которых пуля считается ушедшей в молоко

    int        _squareSide;

    UINT       _bulletType;     // 0 - обычная пуля, 1 - огненная, 2 - ионная, 3 - плазменная, 4 - импульсная, 5 - замораживающая
    bool       _piercing;       // false - пуля при попадании в монстра исчезает, true - пуля пронизывает монстра, теряя часть своей жизни (true при огненных пулях и гауссовом оружии)
    bool       _Shot, _bX, _bY; // флаги для отрисовки хвоста пули при попадании в монстра
};

#endif