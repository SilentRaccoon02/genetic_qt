#include "gamefield.h"
#include "Common/defines.h"

#include <QDebug>
#include <QKeyEvent>
#include <QOpenGLFunctions>
#include <QPainter>

GameField::GameField(QWidget *parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setSamples(4);
    setFormat(format);

    setFixedSize(FIELD_SIZE_X, FIELD_SIZE_Y);
    setFocusPolicy(Qt::StrongFocus);
}

void GameField::slotGameStatus(GameStatus gameStatus)
{
    _gameStatus = gameStatus;
}

void GameField::slotRender(Render render)
{
    _render = render;

    if (_keyWPressed)
        emit sigKeyPressed(Key::W);

    if (_keyAPressed)
        emit sigKeyPressed(Key::A);

    if (_keySPressed)
        emit sigKeyPressed(Key::S);

    if (_keyDPressed)
        emit sigKeyPressed(Key::D);

    repaint();
}

void GameField::slotField(Field field)
{
    _field = field;
}

void GameField::initializeGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void GameField::paintEvent(QPaintEvent * /*paintEvent*/)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);

    QPen pen;
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLines(_render.trackLines);

    if (_field.showChecks)
        painter.drawLines(_render.trackChecks);

    if (_field.showNavLines)
        painter.drawLines(_render.carNavLines);

    if (_gameStatus == GameStatus::Collects)
        painter.drawRect(0, 0, width(), height());

    if (_render.carPoints.size() == 4)
    {
        painter.setBrush(Qt::black);

        QPolygon polygon;
        polygon << QPoint(_render.carPoints[0].x(), _render.carPoints[0].y());
        polygon << QPoint(_render.carPoints[1].x(), _render.carPoints[1].y());
        polygon << QPoint(_render.carPoints[2].x(), _render.carPoints[2].y());
        polygon << QPoint(_render.carPoints[3].x(), _render.carPoints[3].y());
        painter.drawPolygon(polygon);
    }

    if (_field.showNavPoints)
    {
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);

        for (auto const &point : qAsConst(_render.carNavPoints))
            painter.drawEllipse(point, 4, 4);
    }

    painter.end();
}

void GameField::keyPressEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_W)
        _keyWPressed = true;

    if (keyEvent->key() == Qt::Key_A)
        _keyAPressed = true;

    if (keyEvent->key() == Qt::Key_S)
        _keySPressed = true;

    if (keyEvent->key() == Qt::Key_D)
        _keyDPressed = true;
}

void GameField::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_W)
        _keyWPressed = false;

    if (keyEvent->key() == Qt::Key_A)
        _keyAPressed = false;

    if (keyEvent->key() == Qt::Key_S)
        _keySPressed = false;

    if (keyEvent->key() == Qt::Key_D)
        _keyDPressed = false;
}

void GameField::mousePressEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
        emit sigLeftClick(QPointF(mouseEvent->x(), mouseEvent->y()));

    if (mouseEvent->button() == Qt::RightButton)
        emit sigRightClick(QPointF(mouseEvent->x(), mouseEvent->y()));
}
