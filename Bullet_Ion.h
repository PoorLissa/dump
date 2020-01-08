#pragma once
#ifndef _BULLET_ION_H_
#define _BULLET_ION_H_

#include "Bullet.h"

// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ������ ����
// ������������ ���� ��������� �����, �.�. � �.�. ������� ������ ������� ��������, � �� ������� �������� ������� � ������� ������ Bullet::Move()
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
