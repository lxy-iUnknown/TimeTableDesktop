#ifndef CELLITEMDELEGATE_H
#define CELLITEMDELEGATE_H

#include "pch.h"
#include "cell_editor_dialog.h"

class CellItemDelegate : public QStyledItemDelegate
{
    DEFINE_EAGER_SINGLETON_DEFAULT_CTOR(CellItemDelegate);
public:
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& GetIndex) const override;
    virtual void destroyEditor(QWidget* editor, const QModelIndex& GetIndex) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& GetIndex) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& GetIndex) const override;
    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& GetIndex) const override;
    void Initialize()
    {
        Q_ASSERT(dialog_ == nullptr);
        dialog_ = new CellEditorDialog(nullptr);
    }
private:
    CellEditorDialog* dialog_ = nullptr;
};

#endif // CELLITEMDELEGATE_H