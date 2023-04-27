#ifndef TABLEITEMSELECTIONMODEL_H
#define TABLEITEMSELECTIONMODEL_H

#include "pch.h"
#include "singleton.h"
#include "time_table_model.h"

class TimeTableItemSelectionModel : public QItemSelectionModel
{
    DEFINE_EAGER_SINGLETON_DEFAULT_CTOR(TimeTableItemSelectionModel);
public:
    void Initialize()
    {
        Q_ASSERT(model() == nullptr);
        setModel(&TimeTableModel::Instance());
    }
    int Column() const noexcept
    {
        return column_;
    }
    int FirstRow() const noexcept
    {
        return first_row_;
    }
    int LastRow() const noexcept
    {
        return last_row_;
    }
    void select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command) override;
private:
    quint8 column_, first_row_, last_row_;
};

#endif // TABLEITEMSELECTIONMODEL_H
