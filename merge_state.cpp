#include "pch.h"
#include "merge_state.h"

void MergeState::setMergeState(int firstRow, int lastRow, bool merge) NO_EXCEPT
{
    constexpr int RealRowCount = TableUtility::RowCount - 1;

    Q_ASSERT(firstRow >= 0 && firstRow < TableUtility::RowCount);
    Q_ASSERT(lastRow >= 0 && lastRow < TableUtility::RowCount);
    Q_ASSERT(firstRow < lastRow);
    // 0b001_1110_0000
    // 0b001_1111_1111
    // 0b000_0001_1111
    ValueType mask = (1 << (RealRowCount - firstRow)) - (1 << (RealRowCount - lastRow));
    if (merge)
    {
        m_value |= mask;
    }
    else
    {
        m_value &= ~mask;
    }
}
