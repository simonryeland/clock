#include "window.h"

#include <QApplication>

int main (int argc, char **argv)
{
    // Проверяем версию: в Qt 6 этот атрибут включен по умолчанию и помечен как устаревший
#if QT_VERSION < QT_VERSION_CHECK (6, 0, 0)
    QCoreApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute (Qt::AA_UseHighDpiPixmaps); // Для четких иконок
#endif

    QApplication a (argc, argv);
    Window w;
    w.show ();
    return a.exec ();
}
