#include "pch.h"
#include "time_table_item_selection_model.h"

INIT_EAGER_SINGLETON(TimeTableItemSelectionModel);

void TimeTableItemSelectionModel::select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command)
{
    Q_ASSERT(model() != nullptr);
    Q_ASSERT(!selection.isEmpty());
    if (command.testFlag(QItemSelectionModel::SelectionFlag::Rows))
    {
        first_row_ = last_row_ = column_ = 0;
        return;
    }
    if (command.testFlag(QItemSelectionModel::SelectionFlag::NoUpdate))
    {
        QItemSelectionModel::select(selection, command);
        return;
    }
    const auto& first = selection.constFirst();
    if (command.testFlag(QItemSelectionModel::SelectionFlag::Columns))
    {
        first_row_ = 0;
        last_row_ = Constants::kRowCount - 1;
        column_ = static_cast<quint8>(first.left());
    }
    else
    {
        int column;
        const auto& last = selection.size() == 1 ? first : selection.constLast();
        if ((column = first.left()) != last.right())
        {
            first_row_ = last_row_ = column_ = 0;
            return;
        }
        last_row_ = static_cast<quint8>(last.bottom());
        first_row_ = static_cast<quint8>(first.top());
        column_ = static_cast<quint8>(column);
    }
    QItemSelectionModel::select(selection, command);
}
