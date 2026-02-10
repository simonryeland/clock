#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QPoint>

class Window : public QWidget
{
    Q_OBJECT

public:
    Window (QWidget *parent = nullptr);
    ~Window ();

private:
    QPoint m_dragPosition;
    bool isFixed = false;

protected:
    void paintEvent (QPaintEvent *event) override;
    void mousePressEvent (QMouseEvent *event) override;
    void mouseMoveEvent (QMouseEvent *event) override;
    void keyPressEvent (QKeyEvent *event) override;
    void contextMenuEvent (QContextMenuEvent *event) override;

};

#endif // WINDOW_H
