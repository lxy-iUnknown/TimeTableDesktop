#ifndef UTILITY_H
#define UTILITY_H

#include "pch.h"

namespace Utility
{
    template <typename T>
    static T ConstExprError(const char* message)
    {
        Q_UNUSED(message);
        return {};
    }
}

#endif // UTILITY_H
