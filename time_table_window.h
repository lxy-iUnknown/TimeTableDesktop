#ifndef TABLEWINDOW_H
#define TABLEWINDOW_H

#include "pch.h"
#include "cell.h"
#include "cell_item_delegate.h"
#include "time_table_item_selection_model.h"
#include "merged_rows.h"
#include "merge_state.h"
#include "constants.h"
#include "time_table.h"
#include "time_table_model.h"
#include "time_table_window_ui.h"

class TimeTableWindow : public QMainWindow
{
public:
    TimeTableWindow(QWidget *parent = nullptr);

    void ParseArguments(const QApplication& app);
    ~TimeTableWindow()
    {
        delete ui_;
    }
private:
    enum class MenuIndex : quint8
    {
        kSingleItem,
        kAllUnmerged,
        kAllMerged,
        kPartiallyMerged,
    };
    class MergeInfo
    {
    private:
        static constexpr auto kIndexIndex = MergedRows::kMaxCount;
        static constexpr auto kLengthIndex = kIndexIndex + 1;
    public:
        MergeInfo()
        {
            data_[kLengthIndex] = 0;
        }
        void SetIndex(MenuIndex index)
        {
            data_[kIndexIndex] = static_cast<quint8>(index);
        }
        void AddMergedRow(int row)
        {
            quint8 length = data_[kLengthIndex];
            Q_ASSERT(row >= 0 && row < Constants::kRowCount);
            Q_ASSERT(length + 1 < MergedRows::kMaxCount);
            data_[length] = row;
            data_[kLengthIndex] = length + 1;
        }
        MenuIndex GetIndex() const noexcept
        {
            return static_cast<MenuIndex>(data_[kIndexIndex]);
        }
        const auto GetMergedRowCount() const noexcept
        {
            return data_[kLengthIndex];
        }
        const auto& GetMergedRows() const noexcept
        {
            return data_;
        }
    private:
        std::array<quint8, MergedRows::kMaxCount + 1 /* index */ + 1 /* length */> data_;
    };
    ALWAYS_INLINE void InitializeComponent();
    ALWAYS_INLINE void ConnectSignals();
    ALWAYS_INLINE void InitializeData()
    {
        called_from_program_ = true;
        ui_->date_edit_begin_date->setDate(CurrentDate());
        SetModified(false);
        setWindowFilePath(Constants::kUntitled);
        current_file_ = Constants::kEmptyString;
    }
    static void InitializeHeaderView(QHeaderView* header)
    {
        header->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
        header->setDefaultAlignment(Qt::AlignCenter);
    }
    static bool ShouldAcceptDragDropEvent(QDropEvent* event)
    {
        return ShouldAcceptDragDropEvent(event->mimeData());
    }
    static bool ShouldAcceptDragDropEvent(const QMimeData* data)
    {
        return data->hasUrls();
    }
    static void HandleDropEvent(QDropEvent* event)
    {
        if (ShouldAcceptDragDropEvent(event))
        {
            event->acceptProposedAction();
        }
    }
    static QDate FromJulianDayUnsafe(quint64 julian_day)
    {
        QDate date = *reinterpret_cast<QDate*>(&julian_day);
        Q_ASSERT(date.isValid());
        return date;
    }
    ALWAYS_INLINE QDate CurrentDate()
    {
    #ifdef Q_OS_WIN
        static constexpr quint64 Epoch = DateUtility::JulianDayFromYMD(1601, 1, 1);
        static constexpr quint64 Dividend = 24ULL * 60 * 60 * 1000 * 1000 * 10;

        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        ULARGE_INTEGER li;
        li.HighPart = ft.dwHighDateTime;
        li.LowPart = ft.dwLowDateTime;
        QDate date = FromJulianDayUnsafe(li.QuadPart / Dividend + Epoch);
    #else
        QDate date = QDate::currentDate();
    #endif
        if (date < Constants::kMinDate)
        {
            date = Constants::kMinDate;
        }
        if (date > Constants::kMaxDate)
        {
            date = Constants::kMaxDate;
        }
        return date;
    }
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void dragMoveEvent(QDragMoveEvent* event) override;
    virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
    virtual void dropEvent(QDropEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;
    MergeInfo ComputeMergeInfo(int first_row, int last_row, int column);
    void ClearMergedCells(int column, const MergeInfo& info)
    {
        const auto& GetMergedRows = info.GetMergedRows();
        for (int i = 0; i < info.GetMergedRowCount(); i++)
        {
            UnmergeCell(GetMergedRows[i], column);
        }
    }
    void MergeCell(int first_row, int last_row, int column)
    {
        ClearCell(first_row + 1, last_row, column);
        ui_->table_data->setSpan(first_row, column, last_row - first_row + 1, 1);
        TimeTable::Instance().MergeCells(first_row, last_row, column);
    }
    void UnmergeCell(int row, int column)
    {
        int row_span = ui_->table_data->rowSpan(row, column);
        Q_ASSERT(row_span > 1);
        ui_->table_data->setSpan(row, column, 1, 1);
        TimeTable::Instance().UnmergeCells(row, row + row_span - 1, column);
    }
    void ClearCell(int row, int column)
    {
        TimeTable::Instance().GetCell(TimeTable::IndexFromRowColumn(row, column)).Clear();
    }
    void ClearCell(int first_row, int last_row, int column)
    {
        for (int row = first_row; row <= last_row; row++)
        {
            ClearCell(row, column);
        }
    }
    void SetModified(bool modified)
    {
        need_to_save_ = modified;
        setWindowModified(modified);
    }
    bool LoadFile(const QString& file_name);
    ALWAYS_INLINE bool Save()
    {
        if (current_file_.isEmpty())
        {
            return SaveAs();
        }
        else
        {
            return SaveFile(current_file_);
        }
    }
    ALWAYS_INLINE bool SaveAs()
    {
        QString file_name = QFileDialog::getSaveFileName(this,
            Constants::kEmptyString, application_path_, Constants::kEmptyString);
        return !file_name.isEmpty() && SaveFile(file_name);
    }
    ALWAYS_INLINE bool MayBeSave()
    {
        if (!need_to_save_)
        {
            return true;
        }
        switch (QMessageBox::warning(this, Constants::kWarning, Constants::kPromptToSave,
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes))
        {
        case QMessageBox::Yes:
            return Save();
        case QMessageBox::No:
            return true;
        case QMessageBox::Cancel:
            return false;
        default:
            Q_UNREACHABLE();
        }
    }
    bool SaveFile(const QString& fileName);
    void SetCurrentFile(const QString& fileName)
    {
        SetModified(false);
        if (fileName.isEmpty())
        {
            setWindowFilePath(Constants::kUntitled);
        }
        else
        {
            setWindowFilePath(fileName);
        }
        current_file_ = fileName;
    }
    Ui::TimeTableWindow* ui_;
    QString current_file_;
    QString application_path_;
    bool need_to_save_ = false;
    bool called_from_program_ = false;
};

#endif // TABLEWINDOW_H
