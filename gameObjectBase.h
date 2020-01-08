#pragma once
#ifndef _GAME_OBJECT_BASE_H_
#define _GAME_OBJECT_BASE_H_

#include "__highPrecTimer.h"
#include "__threadPool.h"

#define cfRef  const float &
#define ciRef  const int   &
#define cuiRef const unsigned int &

// ------------------------------------------------------------------------------------------------------------------------



// Базовый класс для всякого игрового объекта
class gameObjectBase {

 public:
    gameObjectBase(cfRef x, cfRef y, cfRef scale, cfRef angle, cfRef speed, cuiRef health) :
		_X(x),
		_Y(y),
        _Alive(true),
		_Angle(angle),
		_Speed(speed),
		_Scale(scale),
        _Health(health),
        _HealthMax(health)
	{}
    virtual ~gameObjectBase() {}

    // --- Базовые методы, которые не переопределяются в классах-наследниках ---
    inline const float& getPosX   () const    {    return _X;   }
	inline const float& getPosY   () const    {    return _Y;   } 
    inline const float& getAngle  () const    { return _Angle;  }
	inline const float& getScale  () const    { return _Scale;  }
    inline const int  & getHealth () const    { return _Health; }
    inline const bool & isAlive   () const    { return _Alive;  }

    inline void  setPosX   (const float &x)   {        _X = x;  }
    inline void  setPosY   (const float &y)   {        _Y = y;  }
    inline void  setAngle  (const float &a)   {    _Angle = a;  }
	inline void  setScale  (const float &s)   {    _Scale = s;  }
    inline void  setHealth (const int   &h)   {   _Health = h;  }
    inline void  setAlive  (const bool  &b)   {    _Alive = b;  }

    // Перегрузим оператор сравнения для сортировки списка (монстров). Кроме этого потребуется предикат сортировки, если мы хотим сортировать список указателей
    // http://stackoverflow.com/questions/6404160/sort-a-stdlistmyclass-with-myclassoperatormyclass-other
    inline const bool operator < (const gameObjectBase &other) const { return this == &other ? false : _X < other._X; }

    inline static void setThreadPool(ThreadPool *thPool)             { _thPool = thPool; }

    // --- Виртуальные методы, уникальные для каждого класса-потомка ---

    // метод для обработки объекта в пределах одного фрейма, вызывается в общем цикле. через void* может принимать на вход любые требуемые параметры
    virtual void Move(cfRef = 0, cfRef = 0, void* = nullptr) = 0;

    // метод для получения текущей фазы анимации объекта
    inline virtual cuiRef getAnimPhase() const = 0;

 protected:
    bool            _Alive;
    float           _X, _Y;
    float           _Speed;
    float           _Angle;
	float		    _Scale;
    unsigned int    _Health, _HealthMax;

    static ThreadPool *_thPool;		// указатель пул потоков, на котором будут запускаться многопоточные вычисления
};

#endif
