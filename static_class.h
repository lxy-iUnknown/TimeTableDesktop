#ifndef STATICCLASS_H
#define STATICCLASS_H

#include "pch.h"

class StaticClass
{
private:
    StaticClass() = delete;

    Q_DISABLE_COPY_MOVE(StaticClass);
};

#endif // STATICCLASS_H