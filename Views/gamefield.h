#pragma once

#include <QOpenGLWidget>

#include "Common/structs.h"

class GameField : public QOpenGLWidget
{
    Q_OBJECT

public:
    GameField(QWidget* parent);

signals:
    void sigLeftClick(QPointF point);
    void sigRightClick(QPointF point);
    void sigKeyPressed(Key key);

public slots:
    void slotGameStatus(GameStatus gameStatus);
    void slotRender(Render render);
    void slotField(Field field);

protected:
    void initializeGL() override;
    void paintEvent(QPaintEvent* paintEvent) override;
    void keyPressEvent(QKeyEvent* keyEvent) override;
    void keyReleaseEvent(QKeyEvent* keyEvent) override;
    void mousePressEvent(QMouseEvent* mouseEvent) override;

private:
    GameStatus _gameStatus = GameStatus::Empty;
    Render _render;
    Field _field;

    bool _keyWPressed = false;
    bool _keyAPressed = false;
    bool _keySPressed = false;
    bool _keyDPressed = false;
};
