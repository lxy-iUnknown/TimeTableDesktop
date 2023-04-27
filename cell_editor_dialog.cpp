#include "pch.h"
#include "cell_editor_dialog.h"

void CellEditorDialog::closeEvent(QCloseEvent* event)
{
    if (isWindowModified())
    {
        switch (QMessageBox::warning(this, Constants::kWarning, Constants::kPromptToSave,
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes))
        {
        case QMessageBox::No:
            setResult(QDialog::Rejected);
            break;
        case QMessageBox::Yes:
            setResult(QDialog::Accepted);
            break;
        case QMessageBox::Cancel:
            event->ignore();
            return;
        default:
            Q_UNREACHABLE();
        }
    }
    event->accept();
}
