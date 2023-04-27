#ifndef CELL_EDITOR_DIALOG_UI_H
#define CELL_EDITOR_DIALOG_UI_H

#include "pch.h"
#include "constants.h"
#include "literal.h"

namespace Ui
{
    class CellEditorDialog
    {
    public:
        QDialogButtonBox *button_box;
        QLineEdit *line_edit_odd;
        QLineEdit *line_edit_even;

        void setupUi(QDialog *cell_editor_dialog)
        {
            SET_OBJECT_NAME(cell_editor_dialog);
            cell_editor_dialog->resize(300, 113);
            cell_editor_dialog->setModal(true);
            QGridLayout *grid_layout = new QGridLayout(cell_editor_dialog);
            SET_OBJECT_NAME(grid_layout);
            button_box = new QDialogButtonBox(cell_editor_dialog);
            SET_OBJECT_NAME(button_box);
            button_box->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

            grid_layout->addWidget(button_box, 2, 0, 1, 2);

            QLabel *label_odd = new QLabel(cell_editor_dialog);
            SET_OBJECT_NAME(label_odd);
            label_odd->setTextFormat(Qt::PlainText);

            grid_layout->addWidget(label_odd, 0, 0, 1, 1);

            QLabel *label_even = new QLabel(cell_editor_dialog);
            SET_OBJECT_NAME(label_even);
            label_even->setTextFormat(Qt::PlainText);

            grid_layout->addWidget(label_even, 1, 0, 1, 1);

            line_edit_odd = new QLineEdit(cell_editor_dialog);
            SET_OBJECT_NAME(line_edit_odd);
            line_edit_odd->setMaxLength(Constants::kMaxStringLength);
            line_edit_odd->setClearButtonEnabled(true);
            grid_layout->addWidget(line_edit_odd, 0, 1, 1, 1);

            line_edit_even = new QLineEdit(cell_editor_dialog);
            SET_OBJECT_NAME(line_edit_even);
            line_edit_even->setMaxLength(Constants::kMaxStringLength);
            line_edit_even->setClearButtonEnabled(true);

            grid_layout->addWidget(line_edit_even, 1, 1, 1, 1);

            cell_editor_dialog->setWindowTitle(STRING_LITERAL("编辑课程[*]"));
            label_odd->setText(STRING_LITERAL("单周课程："));
            label_even->setText(STRING_LITERAL("双周课程："));
            line_edit_odd->setPlaceholderText(STRING_LITERAL("单周课程名称"));
            line_edit_even->setPlaceholderText(STRING_LITERAL("双周课程名称"));
        }
    };
}

QT_END_NAMESPACE

#endif // CELL_EDITOR_DIALOG_UI_H
