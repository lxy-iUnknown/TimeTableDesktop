#ifndef MERGEDROWS_H
#define MERGEDROWS_H

#include "pch.h"
#include "merged_row.h"
#include "constants.h"

class MergedRows
{
public:
    static constexpr auto kMaxCount = Constants::kRowCount / 2;

    MergedRows() noexcept = default;
    constexpr void add(quint8 first_row, quint8 last_row) noexcept
    {
        add({ first_row, last_row });
    }
    constexpr void add(const MergedRow& merged_row) noexcept
    {
        merged_rows_[length_++] = merged_row;
    }
    constexpr const MergedRow& operator[](int GetIndex) const noexcept
    {
        return merged_rows_[GetIndex];
    }
    constexpr quint8 length() const noexcept
    {
        return length_;
    }
private:
    quint8 length_;
    std::array<MergedRow, kMaxCount> merged_rows_;
};

#endif // MERGEDROWS_H