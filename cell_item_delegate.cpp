#include "pch.h"
#include "cell_item_delegate.h"

INIT_EAGER_SINGLETON(CellItemDelegate);

QWidget* CellItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& GetIndex) const
{
    Q_UNUSED(option);
    Q_UNUSED(GetIndex);
    Q_ASSERT(dialog_ != nullptr);
    dialog_->setParent(parent, Qt::Dialog);
    dialog_->setWindowFlags(dialog_->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    return dialog_;
}

void CellItemDelegate::destroyEditor(QWidget* editor, const QModelIndex& GetIndex) const
{
    Q_UNUSED(editor);
    Q_UNUSED(GetIndex);
    Q_ASSERT(dialog_ != nullptr);
}

void CellItemDelegate::setEditorData(QWidget* editor, const QModelIndex& GetIndex) const
{
    Q_UNUSED(editor);
    Q_ASSERT(dialog_ != nullptr);
    reinterpret_cast<CellEditorDialog*>(editor)->SetEditorData(GetIndex);
}

void CellItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& GetIndex) const
{
    Q_UNUSED(editor);
    Q_UNUSED(model);
    Q_ASSERT(dialog_ != nullptr);
    CellEditorDialog* dialog = reinterpret_cast<CellEditorDialog*>(editor);
    if (dialog->result() == QDialog::DialogCode::Accepted)
    {
        dialog->SetModelData(GetIndex);
    }
}

void CellItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& GetIndex) const
{
    Q_UNUSED(editor);
    Q_UNUSED(GetIndex);
    Q_ASSERT(dialog_ != nullptr);
    QRect rect = option.rect;
    rect.setTopLeft(editor->parentWidget()->mapToGlobal(rect.topLeft()));
    editor->setGeometry(rect);
}