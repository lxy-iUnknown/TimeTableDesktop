#include "pch.h"
#include "time_table_window.h"

TimeTableWindow::TimeTableWindow(QWidget* parent) :
    QMainWindow(parent),
    application_path_(QApplication::applicationDirPath())
{
    ui_ = new Ui::TimeTableWindow();
    ui_->setupUi(this);
    InitializeComponent();
    ConnectSignals();
    InitializeData();
}

void TimeTableWindow::InitializeComponent()
{
    TimeTableItemSelectionModel::Instance().Initialize();
    CellItemDelegate::Instance().Initialize();

    ui_->table_data->setModel(&TimeTableModel::Instance());
    ui_->table_data->setItemDelegate(&CellItemDelegate::Instance());
    ui_->table_data->setSelectionModel(&TimeTableItemSelectionModel::Instance());
}

void TimeTableWindow::ConnectSignals()
{
    connect(&CellItemDelegate::Instance(), &CellItemDelegate::commitData, this, [&](auto widget) {
        if (reinterpret_cast<CellEditorDialog*>(widget)->result() == QDialog::Accepted)
        {
            SetModified(true);
        }
    });
    connect(ui_->menu_file, &QMenu::triggered, this, [&](auto action) {
        if (action == ui_->action_new)
        {
            if (MayBeSave())
            {
                InitializeData();
                TimeTable::Instance().Clear();
                ui_->table_data->clearSpans();
            }
        }
        else if (action == ui_->action_open)
        {
            if (MayBeSave())
            {
                QString file = QFileDialog::getOpenFileName(this, Constants::kEmptyString,
                    application_path_, Constants::kEmptyString);
                if (!file.isNull())
                {
                    LoadFile(file);
                }
            }
        }
        else if (action == ui_->action_save)
        {
            Save();
        }
        else if (action == ui_->action_save_as)
        {
            SaveAs();
        }
        else if (action == ui_->action_quit)
        {
            close();
        }
        else
        {
            Q_ASSERT(action == nullptr);
        }
    });
    connect(ui_->menu_help, &QMenu::triggered, this, [&](auto action) {
        if (action == ui_->action_about)
        {
            QMessageBox::about(this, STRING_LITERAL("关于课程表"), STRING_LITERAL("课程表用于查看和编辑课表文件"));
        }
        else if (action == ui_->action_about_qt)
        {
            QMessageBox::aboutQt(this, Constants::kEmptyString);
        }
        else
        {
            Q_ASSERT(action == nullptr);
        }
    });
    connect(ui_->table_data, &QTableView::customContextMenuRequested, this, [&](const auto& pos) {
        const auto& selection = TimeTableItemSelectionModel::Instance();
        int first_row = selection.FirstRow();
        int last_row = selection.LastRow();
        int column = selection.Column();
        MergeInfo info = ComputeMergeInfo(first_row, last_row, column);
        QMenu* menu;
        switch (info.GetIndex()) {
        case MenuIndex::kSingleItem:
            menu = ui_->menu_single_item;
            break;
        case MenuIndex::kAllUnmerged:
            menu = ui_->menu_all_unmerged;
            break;
        case MenuIndex::kAllMerged:
            menu = ui_->menu_all_merged;
            break;
        case MenuIndex::kPartiallyMerged:
            menu = ui_->menu_partially_unmerged;
            break;
        default:
            Q_UNREACHABLE();
        }
        QAction* action = menu->exec(ui_->table_data->mapToGlobal(pos));
        if (action == ui_->action_clear_single_item)
        {
            ClearCell(first_row, column);
        }
        else if (action == ui_->action_clear_all_items)
        {
            ClearCell(first_row, last_row, column);
        }
        else if (action == ui_->action_merge)
        {
            ClearMergedCells(column, info);
            MergeCell(first_row, last_row, column);
        }
        else if (action == ui_->action_unmerge)
        {
            ClearMergedCells(column, info);
        }
        else if (action == ui_->action_clear_and_unmerge)
        {
            ClearMergedCells(column, info);
            ClearCell(first_row, last_row, column);
        }
        else
        {
            Q_ASSERT(action == nullptr);
        }
        SetModified(true);
    });
    connect(ui_->date_edit_begin_date, &QDateEdit::userDateChanged, this, [&](const QDate& begin_date) {
        QDate now = CurrentDate();
        int week_count = DateUtility::WeekCount(begin_date, now);
        TimeTableModel::Instance().SetIsEven((week_count % 2) == 0);
        ui_->label_info->setText(STRING_VIEW_LITERAL("开学日期：%1（第%2周）")
                                 .arg(now.toString(Qt::DateFormat::ISODate)).arg(week_count));
        SetModified(!called_from_program_);
        if (called_from_program_)
        {
            called_from_program_ = false;
        }
    });
}

void TimeTableWindow::ParseArguments(const QApplication& app)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
#define FILE "文件"
    parser.addPositionalArgument(STRING_LITERAL(FILE), STRING_LITERAL("课表文件（可选）"), STRING_LITERAL("[" FILE "]..."));
#undef FILE
    parser.process(app);
    auto arguments = parser.positionalArguments();
    if (!arguments.isEmpty())
    {
        LoadFile(arguments[0]);
    }
}

void TimeTableWindow::dragEnterEvent(QDragEnterEvent* event)
{
    HandleDropEvent(event);
}

void TimeTableWindow::dragMoveEvent(QDragMoveEvent* event)
{
    HandleDropEvent(event);
}

void TimeTableWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void TimeTableWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* data = event->mimeData();
    if (ShouldAcceptDragDropEvent(data))
    {
        QList<QUrl> urlList = data->urls();
        if (LoadFile(urlList[0].toLocalFile()))
        {
            event->acceptProposedAction();
        }
    }
}

void TimeTableWindow::closeEvent(QCloseEvent* event)
{
    if (MayBeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

TimeTableWindow::MergeInfo TimeTableWindow::ComputeMergeInfo(int first_row, int last_row, int column)
{
    MergeInfo info;
    if (last_row == first_row)
    {
        info.SetIndex(MenuIndex::kSingleItem);
        return info;
    }
    int rowCount = last_row - first_row + 1;
    if (ui_->table_data->rowSpan(first_row, column) == rowCount)
    {
        info.AddMergedRow(first_row);
        info.SetIndex(MenuIndex::kAllMerged);
        return info;
    }
    int row = first_row;
    while (row < last_row)
    {
        int row_span = ui_->table_data->rowSpan(row, column);
        if (row_span > 1)
        {
            info.AddMergedRow(row);
        }
        row += row_span;
    }
    info.SetIndex(info.GetMergedRowCount() == 0 ? MenuIndex::kAllUnmerged : MenuIndex::kPartiallyMerged);
    return info;
}

bool TimeTableWindow::LoadFile(const QString& file_name)
{
    using namespace std::chrono;

    QString absolute_name;
    QFileInfo info(file_name);
    if (info.makeAbsolute())
    {
        absolute_name = info.absoluteFilePath();
    }
    else
    {
        absolute_name = file_name;
    }
    QFile file(absolute_name);
    if (!file.open(QIODevice::OpenModeFlag::ReadOnly))
    {
        QMessageBox::critical(this, Constants::kError, STRING_VIEW_LITERAL("打开课表文件\"%1\"失败，%2")
                              .arg(file_name, file.errorString()));
        return false;
    }
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    stream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    qint32 date = TimeTable::Instance().Deserialize(stream);
    if (date < 0)
    {
        QMessageBox::critical(this, Constants::kError, STRING_VIEW_LITERAL("保存课表文件\"%1\"无效")
                              .arg(absolute_name));
        return false;
    }
    ui_->table_data->clearSpans();
    for (int column = 0; column < Constants::kColumnCount; column++)
    {
        const auto& GetMergedRows = MergeState::kMergedRowData[TimeTable::Instance().GetMergeState(column).Value()];
        for (int i = 0; i < GetMergedRows.length(); i++)
        {
            const auto& merged_row = GetMergedRows[i];
            ui_->table_data->setSpan(merged_row.first_row(), column, merged_row.last_row() - merged_row.first_row() + 1, 1);
        }
    }
    called_from_program_ = true;
    ui_->date_edit_begin_date->setDate(QDate::fromJulianDay(date + Constants::kMinDateJulianDay));
    SetCurrentFile(file_name);
    return true;
}

bool TimeTableWindow::SaveFile(const QString& file_name)
{
    QSaveFile file(file_name);
    file.setDirectWriteFallback(false);
    if (file.open(QIODevice::OpenModeFlag::WriteOnly))
    {
        QDataStream stream(&file);
        stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
        stream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
        TimeTable::Instance().Serialize(stream, ui_->date_edit_begin_date->date());
        if (file.commit())
        {
            SetCurrentFile(file_name);
            return true;
        }
    }
    QMessageBox::critical(this, Constants::kError, STRING_VIEW_LITERAL("保存课表文件\"%1\"失败，%2")
                          .arg(file_name, file.errorString()));
    return false;
}
