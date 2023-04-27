#ifndef MERGESTATE_H
#define MERGESTATE_H

#include "pch.h"
#include "merged_rows.h"
#include "constants.h"

struct MergeState
{
private:
    using ValueType = quint16;

	static constexpr int kInvalidRow = Constants::kRowCount;
	static constexpr int kBitCount = sizeof(ValueType) * CHAR_BIT;
	
	static_assert(Constants::kRowCount <= kBitCount, "Insufficient bits");
public:

    static constexpr ValueType kMaximum = (1 << (Constants::kRowCount - 1)) - 1;
    static constexpr auto kMergedRowData = ([]() noexcept {
        struct Helper
        {
            static constexpr int LeadingZeroCount(quint32 value)
            {
                constexpr quint8 kDeBruijn[] =
                {
                    0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
                    8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
                };

                if (value == 0)
                {
                    return 32;
                }
                value |= value >> 1;
                value |= value >> 2;
                value |= value >> 4;
                value |= value >> 8;
                value |= value >> 16;
                return 31 - kDeBruijn[((value * 0x07c4acdd) >> 27)];
            }
            static constexpr int LeadingZeroCount(quint64 value)
            {
                quint32 high = static_cast<quint32>(value >> 32);
                if (high == 0)
                {
                    return 32 + LeadingZeroCount(static_cast<quint32>(value));
                }
                return LeadingZeroCount(high);
            }
            static constexpr int LeadingZeroCount(quint16 value)
            {
                return LeadingZeroCount(static_cast<quint32>(value)) - 16;
            }
            static constexpr int LeadingZeroCount(quint8 value)
            {
                return LeadingZeroCount(static_cast<quint32>(value)) - 24;
            }
            static constexpr int TrailingZeroCount(quint32 value)
            {
                constexpr quint8 kDeBruijn[] =
                {
                    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
                    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
                };

                if (value == 0)
                {
                    return 32;
                }
                return kDeBruijn[(value & (static_cast<quint32>(-static_cast<qint32>(value)))) * 0x77cb531 >> 27];
            }
            static constexpr int TrailingZeroCount(quint64 value)
            {
                quint32 low = static_cast<quint32>(value);
                if (low == 0)
                {
                    return 32 + TrailingZeroCount(static_cast<quint32>(value >> 32));
                }
                return TrailingZeroCount(low);
            }
            static constexpr int BitMaskForRow(int row)
            {
                return (1 << (kBitCount - row)) - 1;
            }
            static constexpr int RowIndexFromLeadingZeroCount(int lzcnt)
            {
                return lzcnt - (kBitCount - Constants::kRowCount);
            }
            static constexpr ValueType ChangeState(int lzcnt, ValueType state)
            {
                return (~state) & BitMaskForRow(lzcnt);
            }
            static constexpr ValueType FinalChangeState(int lzcnt, ValueType state)
            {
                return state & BitMaskForRow(lzcnt);
            }
        };

        std::array<MergedRows, kMaximum + 1> merged_row_data = {};
        for (ValueType i = 0; i <= kMaximum; i++)
        {
            ValueType value = i << 1;
            for (; ; )
            {
                int lzcnt = Helper::LeadingZeroCount(value);
                int first_row = Helper::RowIndexFromLeadingZeroCount(lzcnt);
                if (first_row == kInvalidRow)
                {
                    break;
                }
                lzcnt = Helper::LeadingZeroCount(Helper::ChangeState(lzcnt, value));
                int last_row = Helper::RowIndexFromLeadingZeroCount(lzcnt);
                if (last_row == kInvalidRow)
                {
                    break;
                }
                merged_row_data[i].add({ static_cast<quint8>(first_row),
                                         static_cast<quint8>(last_row) });
                value = Helper::FinalChangeState(lzcnt, value);
            }
        }
        return merged_row_data;
    })();

    ValueType Value() const noexcept
    {
        return value_;
    }
    void SetValue(ValueType value)
    {
        Q_ASSERT(value <= kMaximum);
        value_ = value;
    }
    void SetMergeState(int first_row, int last_row, bool merge)
    {
        constexpr int kRealRowCount = Constants::kRowCount - 1;

        Q_ASSERT(first_row >= 0 && first_row < Constants::kRowCount);
        Q_ASSERT(last_row >= 0 && last_row < Constants::kRowCount);
        Q_ASSERT(first_row < last_row);
        // 0b001_1110_0000
        // 0b001_1111_1111
        // 0b000_0001_1111
        ValueType mask = (1 << (kRealRowCount - first_row)) - (1 << (kRealRowCount - last_row));
        if (merge)
        {
            value_ |= mask;
        }
        else
        {
            value_ &= ~mask;
        }
    }
private:
    ValueType value_ = 0;
};

#endif // MERGESTATE_H
