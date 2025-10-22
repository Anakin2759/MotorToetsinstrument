#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QStyleFactory>

#include "controller/MainController.h"
#include "iostream"
#include "view/MainWindow.h"

void customTerminateHandler()
{
    std::cerr << "存在未知异常，程序中止" << std::endl;
    std::abort(); // 调用 std::abort 来终止程序
}

int main(int argc, char *argv[])
{
    std::set_terminate(customTerminateHandler);
    QApplication app(argc, argv);
    //    app.setPalette(QPalette(QPalette::Light));
    //    app.setStyle(QStyleFactory::create("Fusion"));

    QFile styleFile(":qss/flatgray.css"); // 资源路径
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream ts(&styleFile);
        QString style = ts.readAll();
        app.setStyleSheet(style); // 应用样式表
    }
    else
    {
        qWarning("Failed to load the style file.");
    }

    MainWindow window;
    MainController controller(window);

    window.show();

    return app.exec();
}
