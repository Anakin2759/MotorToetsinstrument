#pragma once

#include <QStyledItemDelegate>
#include <QWidget>

class MotorDataStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MotorDataStyledItemDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    //    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    //    override;
};