#include "window.h"
#include <QApplication>
#include <QtGlobal> // Содержит макросы версий
#include <QMenu>
#include <QAction>
#include <QPropertyAnimation>

Window::Window (QWidget *parent) : QWidget (parent) {
    // Объединяем три важных флага:
    // 1. Без рамки
    // 2. Поверх всех окон
    // 3. Инструментальное окно (не отображается в панели задач, опционально)
    setWindowFlags (Qt::FramelessWindowHint |
                    Qt::WindowStaysOnTopHint |
                    Qt::Tool);

    setAttribute (Qt::WA_TranslucentBackground);

    QTimer *timer = new QTimer (this);
    // Вызываем update (), а не само рисование
    connect (timer, &QTimer::timeout, this, QOverload<>::of (&Window::update));
    timer->start (40);
}

Window::~Window () {}

void Window::paintEvent (QPaintEvent *event) {
    Q_UNUSED (event);
    QTime time = QTime::currentTime ();
    QPainter painter (this);
    painter.setRenderHint (QPainter::Antialiasing);

    painter.fillRect(rect (), QColor (0, 0, 0, 1));

    // 1. Сначала считаем базовый динамический цвет
    int hour = time.hour ();
    double dayIntensity = 1.0 - qAbs (hour - 12) / 12.0;
    QColor dynamicColor (50 + (200 * dayIntensity), 150, 200 - (150 * dayIntensity));

    painter.translate (width () / 2, height () / 2);
    int R = 0.8 * qMin (width () / 2, height () / 2);

    // 2. Теперь засечки будут видны, так как динамический цвет уже готов
    painter.setPen (dynamicColor); 
    for (int i = 0; i < 60; ++i) {
        if (i % 5 == 0) painter.drawLine (0, R, 0, R - 10);
        else painter.drawLine (0, R, 0, R - 4);
        painter.rotate (6.0);
    }

    painter.setPen (Qt::NoPen);

    // Часовая стрелка
    double hourAngle = 30.0 * (time.hour () + time.minute () / 60.0 + time.second () / 3600.0);
    painter.save ();
    painter.rotate (hourAngle);
    QPolygon pol;
    pol << QPoint (-0.05 * R, 0) << QPoint (0.05 * R, 0) << QPoint (0, -0.4 * R);
    painter.setBrush (dynamicColor);
    painter.drawPolygon (pol);
    painter.restore ();

    // Минутная стрелка
    double minuteAngle = 6.0 * (time.minute () + time.second() / 60.0);
    painter.save ();
    painter.rotate (minuteAngle);
    QPolygon pol2;
    pol2 << QPoint (-0.05 * R, 0) << QPoint (0.05 * R, 0) << QPoint (0, -0.7 * R);
    QColor minColor = dynamicColor;
    minColor.setAlpha (191);
    painter.setBrush (minColor);
    painter.drawPolygon (pol2);
    painter.restore ();

    // Секундная стрелка
    double secondAngle = 6.0 * (time.second () + time.msec () / 1000.0);
    painter.save ();
    painter.rotate (secondAngle);
    QPolygon pol3;
    pol3 << QPoint (-0.02 * R, 0) << QPoint (0.02 * R, 0) << QPoint (0, -0.9 * R);
    QColor secColor = dynamicColor;
    secColor.setAlpha (127);
    painter.setBrush (secColor);
    painter.drawPolygon (pol3);
    painter.restore ();
}

// Перетаскивание: запоминаем точку нажатия
void Window::mousePressEvent (QMouseEvent *event) {
    if (isFixed) return; // Если часы закреплены, игнорируем перетаскивание

    if (event->button () == Qt::LeftButton) {
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 0)
        // Для Qt 6.0 и выше
        m_dragPosition = event->globalPosition ().toPoint () - frameGeometry ().topLeft ();
#else
        // Для Qt 5.x и ниже
        m_dragPosition = event->globalPos () - frameGeometry ().topLeft ();
#endif
        event->accept ();
    }
}

// Перетаскивание: двигаем окно
void Window::mouseMoveEvent (QMouseEvent *event) {
    if (isFixed) return; // Если часы закреплены, игнорируем перетаскивание

    if (event->buttons () & Qt::LeftButton) {
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 0)
        move (event->globalPosition ().toPoint () - m_dragPosition);
#else
        move (event->globalPos () - m_dragPosition);
#endif
        event->accept ();
    }
}

// Закрытие по нажатию ESC
void Window::keyPressEvent (QKeyEvent *event) {
    if (event->key () == Qt::Key_Escape) {
        qApp->quit(); // Завершает всё приложение, а не просто закрывает окно
    }
}

void Window::contextMenuEvent (QContextMenuEvent *event) {
    QMenu menu (this);

    // Пункт Закрепить
    QAction *fixAction = menu.addAction (isFixed ? "Разблокировать" : "Закрепить");

    menu.addSeparator (); // Разделитель для порядка

    // Пункты изменения размера
    QAction *small = menu.addAction ("Маленькие\t(200x200)");
    QAction *medium = menu.addAction ("Средние\t(400x400)");
    QAction *large = menu.addAction ("Большие\t(600x600)");

    small->setCheckable (true);
    medium->setCheckable (true);
    large->setCheckable (true);

    // Устанавливаем галочку в зависимости от текущей ширины окна
    if (width () == 200) small->setChecked (true);
    else if (width () == 400) medium->setChecked (true);
    else if (width () == 600) large->setChecked (true);

    menu.addSeparator();

    // Пункт Выход
    QAction *quitAction = menu.addAction ("Выход");

    // Запуск меню
    QAction *selectedItem = menu.exec (event->globalPos ());

    auto animateTo = [this](int newSize) {
        QPropertyAnimation *anim = new QPropertyAnimation (this, "geometry");
        anim->setDuration (500); // Скорость анимации в мс (0.3 сек)
        anim->setStartValue (geometry ());
        
        // Центрируем новый размер относительно старого центра
        QRect newRect = geometry ();
        newRect.setSize (QSize (newSize, newSize));
        newRect.moveCenter (geometry ().center ());
        
        anim->setEndValue (newRect);
        anim->setEasingCurve (QEasingCurve::OutCubic); // Плавное замедление в конце
        anim->start (QAbstractAnimation::DeleteWhenStopped);
    };

    if (selectedItem == quitAction) {
        qApp->quit ();
    } else if (selectedItem == fixAction) {
        isFixed = !isFixed;
        update ();
    } else if (selectedItem == small) {
        animateTo (200);
    } else if (selectedItem == medium) {
        animateTo (400);
    } else if (selectedItem == large) {
        animateTo (600);
    }
}
