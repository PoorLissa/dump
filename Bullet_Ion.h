#pragma once
#ifndef _BULLET_ION_H_
#define _BULLET_ION_H_

#include "Bullet.h"

// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Ионная пуля
// Унаследовали свой отдельный класс, т.к. у И.П. немного другой просчет движения, и не хочется городить условия в базовом методе Bullet::Move()
class BulletIon : public Bullet {

 public:
    BulletIon(cfRef x, cfRef y, cfRef scale, cfRef x_to, cfRef y_to, cfRef speed);
   ~BulletIon() {}

    inline virtual void Move(cfRef, cfRef, void *Param) {
        _thPool->runAsync(&BulletIon::threadMove, this);
    }

 private:
    void threadMove();
};

#endif
