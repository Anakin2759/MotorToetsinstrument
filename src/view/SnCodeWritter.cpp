#include <QLineEdit>

#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

#include "SnCodeWritter.h"
#include "view/SnCodeWidget.h"

SnCodeWritter::SnCodeWritter(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    QScrollArea *scrollArea = new QScrollArea;
    QWidget *scrollContent = new QWidget; // 用于滚动区域的内容
    scrollLayout = new QVBoxLayout;

    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true); // 确保滚动区域可调整大小

    savebutton = new QPushButton("保存");
    layout->addWidget(scrollArea);
    layout->addWidget(savebutton);

    setLayout(layout);
}

SnCodeWritter::~SnCodeWritter() {}

void SnCodeWritter::updateUI(const size_t &number)
{
    editList.clear();
    QLayoutItem *item = scrollLayout->itemAt(0);

    // 检查该项是否是 QSpacerItem（即伸缩空间）
    if (QSpacerItem *spacer = dynamic_cast<QSpacerItem *>(item))
    {
        // 如果是伸缩空间，则移除它
        scrollLayout->removeItem(item);
        delete spacer; // 删除该伸缩空间对象
    }
    for (size_t i = 0; i < number; i++)
    {
        // 生成一行代码
        editList.emplace_back(std::make_unique<SnCodeWidget>());
        editList[i]->setSnCodeNumber(i + 1);
        scrollLayout->addWidget(editList[i].get());
    }
    scrollLayout->addStretch();
}
