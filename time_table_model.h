#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include "pch.h"
#include "singleton.h"
#include "constants.h"
#include "time_table.h"

class TimeTableModel : public QAbstractTableModel
{
    DEFINE_EAGER_SINGLETON_DEFAULT_CTOR(TimeTableModel);
public:
    virtual Qt::ItemFlags flags(const QModelIndex& GetIndex) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &GetIndex, int role = Qt::DisplayRole) const override;
    void SetIsEven(bool is_even)
    {
        is_even_ = is_even;
    }
private:
    inline static const QStringView kRowHeader[Constants::kRowCount] =
    {
        u"1", u"2", u"3", u"4", u"5", u"6", u"7", u"8", u"9", u"10", u"11、12", u"13、14"
    };
#define DEFINE_WEEKDAY(weekday) u"星期" weekday
    inline static const QStringView kColumnHeader[Constants::kColumnCount] =
    {
        DEFINE_WEEKDAY("一"), DEFINE_WEEKDAY("二"),  DEFINE_WEEKDAY("三"),  DEFINE_WEEKDAY("四"), DEFINE_WEEKDAY("五")
    };
#undef DEFINE_WEEKDAY
#define DEFINE_TOOLTIP(label) u"上课时间：" label
    inline static const QStringView kTimeValueTooltips[Constants::kRowCount] =
    {
        DEFINE_TOOLTIP("8:30-9:10"), DEFINE_TOOLTIP("9:15-9:55"), DEFINE_TOOLTIP("10:15-11:55"),
        DEFINE_TOOLTIP("11:00-11:40"), DEFINE_TOOLTIP("11:45-12:25"), DEFINE_TOOLTIP("13:30-14:10"),
        DEFINE_TOOLTIP("14:15-14:55"), DEFINE_TOOLTIP("15:00-15:40"), DEFINE_TOOLTIP("16:00-16:40"),
        DEFINE_TOOLTIP("16:45-17:25"), DEFINE_TOOLTIP("19:00-20:20"), DEFINE_TOOLTIP("20:30-21:45")
    };
#undef DEFINE_TOOLTIP

    bool is_even_;
};

#endif // TABLEMODEL_H
