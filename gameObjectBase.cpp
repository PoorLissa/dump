#include "stdafx.h"
#include "gameObjectBase.h"

// Инициализируем статические переменные в глобальной области:

// статический пул потоков семейства объектов gameObjectBase
ThreadPool* gameObjectBase::_thPool = nullptr;
