#include <QApplication>
#include <QLineEdit>
#include <QPainter>
#include <QStyleOptionViewItem>

#include "MotorDataDelegate.h"

MotorDataStyledItemDelegate::MotorDataStyledItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void MotorDataStyledItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
    QString text = index.data(Qt::DisplayRole).toString();

    painter->save();
    // 设置文本颜色
    painter->setPen(Qt::black);                        // 设置文本颜色为蓝色
    painter->setFont(QFont("Arial", 12, QFont::Bold)); // 设置字体样式为粗体
                                                       // 自定义绘制的样式
    if (index.column() == 7)                           // 例如：自定义绘制第一列

    {
        //    painter->setBackground(QBrush(Qt::yellow)); // 设置背景颜色为黄色
        painter->fillRect(option.rect, Qt::yellow);
        if (text == "正常")

        {
            painter->fillRect(option.rect, Qt::green); // 设置文本颜色为绿色
        }
        else if (text == "离线")
        {
            painter->fillRect(option.rect, Qt::red); // 设置文本颜色为黄色
        }
        else if (text == "报警")
        {
            painter->fillRect(option.rect, Qt::red); // 设置文本颜色为红色
        }
        else if (text == "未知")
        {
            painter->fillRect(option.rect, Qt::gray); // 设置文本颜色为灰色
        }
    }
    painter->drawText(option.rect, Qt::AlignCenter, text);
    painter->restore();
}

// QWidget* MotorDataStyledItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
//                                                    const QModelIndex& index) const
// {
//     // 创建一个 QLineEdit 作为编辑器
//     QLineEdit* editor = new QLineEdit(parent);

//     // 设置编辑器的初始文本为当前单元格的数据
//     editor->setText(index.data(Qt::EditRole).toString());

//     // 返回创建的编辑器
//     return editor;
// }
