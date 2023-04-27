#ifndef ROWANDCOLUMN_H
#define ROWANDCOLUMN_H

#include "pch.h"
#include "constants.h"

class RowColumn
{
public:
    inline RowColumn() : RowColumn(0, 0) {}
    inline RowColumn(int row, int column)
    {
        Q_ASSERT(row >= 0 && row < Constants::kRowCount);
        Q_ASSERT(column >= 0 && column < Constants::kColumnCount);
        row_ = static_cast<quint8>(row);
        column_ = static_cast<quint8>(column);
    }
    inline RowColumn(int index)
    {
        Q_ASSERT(index >= 0 && index < Constants::kCellCount);
        int row = index / Constants::kColumnCount;
        int column = index - row * Constants::kColumnCount; // index % column;
        row_ = static_cast<quint8>(row);
        column_ = static_cast<quint8>(column);
    }
    quint8 row() const noexcept
    {
        return row_;
    }
    quint8 column() const noexcept
    {
        return column_;
    }
private:
    quint8 row_, column_;
};

#endif // ROWANDCOLUMN_H