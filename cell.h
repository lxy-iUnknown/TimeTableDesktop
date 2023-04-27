#ifndef CELL_H
#define CELL_H

#include "pch.h"
#include "cell_data.h"
#include "constants.h"

class Cell
{
public:
    const QString& Odd() const
    {
        return odd_;
    }
    const QString& Even() const
    {
        return even_;
    }
    void SetOdd(const QString& odd)
    {
        Q_ASSERT(odd.size() <= Constants::kMaxStringLength);
        odd_ = odd;
    }
    void SetEven(const QString& even)
    {
        Q_ASSERT(even.size() <= Constants::kMaxStringLength);
        even_ = even;
    }
    void Clear()
    {
        odd_.clear();
        even_.clear();
    }
    void SetData(const CellData& data)
    {
        data.odd.CopyTo(odd_);
        data.even.CopyTo(even_);
    }
private:
    QString odd_, even_;
};

#endif // CELL_H