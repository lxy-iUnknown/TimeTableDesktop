#ifndef MERGEDROW_H
#define MERGEDROW_H

#include "pch.h"

class MergedRow
{
public:
    MergedRow() noexcept = default;
    constexpr MergedRow(quint8 first_row, quint8 last_row) noexcept :
        first_row_(first_row), last_row_(last_row) {}
    quint8 first_row() const noexcept
    {
        return first_row_;
    }
    quint8 last_row() const noexcept
    {
        return last_row_;
    }
private:
    quint8 first_row_, last_row_;
};

#endif // MERGEDROW_H