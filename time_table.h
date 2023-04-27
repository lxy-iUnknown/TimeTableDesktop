#ifndef TABLE_H
#define TABLE_H

#include "pch.h"
#include "cell.h"
#include "cell_data.h"
#include "constants.h"
#include "date_utility.h"
#include "merge_state.h"
#include "singleton.h"

class TimeTable
{
    DEFINE_EAGER_SINGLETON_DEFAULT_CTOR(TimeTable);
public:
    static int IndexFromRowColumn(int row, int column)
    {
        Q_ASSERT(row >= 0 && row < Constants::kRowCount);
        Q_ASSERT(column >= 0 && column < Constants::kColumnCount);
        return (int)(row * Constants::kColumnCount + column);
    }

    MergeState& GetMergeState(int column) const
    {
        Q_ASSERT(column >= 0 && column < Constants::kColumnCount);
        return const_cast<MergeState&>(merge_states_[column]);
    }
    Cell& GetCell(int GetIndex) const
    {
        Q_ASSERT(GetIndex >= 0 && GetIndex < Constants::kCellCount);
        return const_cast<Cell&>(cells_[GetIndex]);
    }
    void Clear()
    {
        for (int i = 0; i < Constants::kCellCount; i++)
        {
            cells_[i].Clear();
        }
        memset(merge_states_, 0, sizeof(merge_states_));
    }
    void MergeCells(int first_row, int last_row, int column)
    {
        SetCellMergeState(first_row, last_row, column, true);
    }
    void UnmergeCells(int first_row, int last_row, int column)
    {
        SetCellMergeState(first_row, last_row, column, false);
    }
    void Serialize(QDataStream& stream, const QDate& date) const
    {
        stream << static_cast<quint8>(date.toJulianDay() - Constants::kMinDateJulianDay);
        for (int i = 0; i < Constants::kCellCount; i++)
        {
            SerializeCell(stream, cells_[i]);
        }
        stream.writeRawData(reinterpret_cast<const char*>(merge_states_), sizeof(merge_states_));
    }
    ALWAYS_INLINE qint32 Deserialize(QDataStream& stream)
    {
        constexpr auto kMergedStatesSize = sizeof(merge_states_);

#ifdef QT_DEBUG
        memset(cell_data_, 0, sizeof(cell_data_));
#endif
        quint32 epoch_day;
        stream >> epoch_day;
        MergeState merge_states[Constants::kColumnCount];
        for (int i = 0; i < Constants::kCellCount; i++)
        {
            DeserializeCell(stream, cell_data_[i]);
        }
        if (stream.readRawData(reinterpret_cast<char*>(merge_states), kMergedStatesSize) < kMergedStatesSize
                || epoch_day > Constants::kMaxDateEpochDay)
        {
            goto Error;
        }
        for (int i = 0; i < Constants::kColumnCount; i++)
        {
            if (merge_states[i].Value() > MergeState::kMaximum)
            {
                goto Error;
            }
        }
        for (int i = 0; i < Constants::kCellCount; i++)
        {
            cells_[i].SetData(cell_data_[i]);
        }
        memcpy(merge_states_, merge_states, sizeof(merge_states_));
        return epoch_day;
    Error:
        return Constants::kInvalidDateEpochDay;
    }
private:
    void SetCellMergeState(int first_row, int last_row, int column, bool state)
    {
        Q_ASSERT(column >= 0 && column < Constants::kColumnCount);
        merge_states_[column].SetMergeState(first_row, last_row, state);
    }

    static void ReadString(QDataStream& stream, CellString& str)
    {
        quint8 length = 0;
        stream >> length;
        stream.readRawData(reinterpret_cast<char*>(str.GetData()), length * sizeof(char16_t));
        str.SetLength(length);
    }
    static void WriteString(QDataStream& stream, const QString& str)
    {
        int length = str.length();
        stream << static_cast<quint8>(length);
        stream.writeRawData(reinterpret_cast<const char*>(str.data()), length * sizeof(char16_t));
    }
    static void SerializeCell(QDataStream& stream, const Cell& cell)
    {
        WriteString(stream, cell.Odd());
        WriteString(stream, cell.Even());
    }
    static void DeserializeCell(QDataStream& stream, CellData& data)
    {
        ReadString(stream, data.odd);
        ReadString(stream, data.even);
    }
    Cell cells_[Constants::kCellCount];
    CellData cell_data_[Constants::kCellCount];
    MergeState merge_states_[Constants::kColumnCount];
};

#endif // TABLE_H
