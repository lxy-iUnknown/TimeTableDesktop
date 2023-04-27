#ifndef CELLEDITORDIALOG_H
#define CELLEDITORDIALOG_H

#include "pch.h"
#include "constants.h"
#include "time_table.h"
#include "cell_editor_dialog_ui.h"

class CellEditorDialog : public QDialog
{
public:
    CellEditorDialog(QWidget* parent) : QDialog(parent)
    {
        ui_ = new Ui::CellEditorDialog();
        ui_->setupUi(this);
        InitializeComponent();
        ConnectSignals();
    }
    void SetEditorData(const QModelIndex& GetIndex)
    {
        const auto& cell = TimeTable::Instance().GetCell(
            TimeTable::IndexFromRowColumn(GetIndex.row(), GetIndex.column()));
        ui_->line_edit_odd->setText(cell.Odd());
        ui_->line_edit_even->setText(cell.Even());
    }
    void SetModelData(const QModelIndex& GetIndex)
    {
        if (ui_->line_edit_odd->isModified() || ui_->line_edit_even->isModified())
        {
            auto& cell = TimeTable::Instance().GetCell(
                TimeTable::IndexFromRowColumn(GetIndex.row(), GetIndex.column()));
            cell.SetOdd(ui_->line_edit_odd->text());
            cell.SetEven(ui_->line_edit_even->text());
            setWindowModified(false);
        }
        else
        {
            setResult(QDialog::Rejected);
        }
    }
    ~CellEditorDialog()
    {
        delete ui_;
    }
private:
    ALWAYS_INLINE void InitializeComponent()
    {
        setFixedSize(size());

        ui_->line_edit_odd->setMaxLength(Constants::kMaxStringLength);
        ui_->line_edit_even->setMaxLength(Constants::kMaxStringLength);
    }
    ALWAYS_INLINE void ConnectSignals()
    {
        connect(ui_->button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(ui_->button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
        connect(ui_->line_edit_odd, &QLineEdit::textChanged, this, &CellEditorDialog::UpdateModified);
        connect(ui_->line_edit_even, &QLineEdit::textChanged, this, &CellEditorDialog::UpdateModified);
    }
    void UpdateModified()
    {
        setWindowModified(ui_->line_edit_odd->isModified() || ui_->line_edit_even->isModified());
    }
    virtual void closeEvent(QCloseEvent* event) override;

    Ui::CellEditorDialog* ui_;
};

#endif // CELLEDITORDIALOG_H
