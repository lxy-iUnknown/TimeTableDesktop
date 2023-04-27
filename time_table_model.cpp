#include "pch.h"
#include "time_table_model.h"

INIT_EAGER_SINGLETON(TimeTableModel);

static QString FromStringView(QStringView view)
{
    return QString::fromRawData(view.data(), view.size());
}

Qt::ItemFlags TimeTableModel::flags(const QModelIndex& GetIndex) const
{
    Q_UNUSED(GetIndex);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant TimeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        return FromStringView((orientation == Qt::Orientation::Horizontal ? kColumnHeader : kRowHeader)[section]);
    case Qt::ToolTipRole:
        if (orientation == Qt::Vertical)
        {
            return FromStringView(kTimeValueTooltips[section]);
        }
    }
    return QVariant();
}

int TimeTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return Constants::kRowCount;
}

int TimeTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return Constants::kColumnCount;
}

QVariant TimeTableModel::data(const QModelIndex &GetIndex, int role) const
{
    if (GetIndex.isValid())
    {
        const auto& cell = TimeTable::Instance().GetCell(
            TimeTable::IndexFromRowColumn(GetIndex.row(), GetIndex.column()));
        switch (role)
        {
        case Qt::DisplayRole:
            return is_even_ ? cell.Even() : cell.Odd();
        case Qt::ToolTipRole:
            return STRING_VIEW_LITERAL("单周课程：%1\n双周课程：%2").arg(cell.Odd(), cell.Even());
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        }
    }
    return QVariant();
}
