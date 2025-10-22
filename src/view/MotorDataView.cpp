#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QTableView>

#include "MotorDataDelegate.h"
#include "MotorDataView.h"

MotorDataView::MotorDataView(QWidget* parent) : QTableView(parent)
{
    MotorDataStyledItemDelegate* delegate = new MotorDataStyledItemDelegate();
    setItemDelegate(delegate);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}