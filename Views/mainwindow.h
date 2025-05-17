#pragma once

#include <QMainWindow>

#include "Common/structs.h"
#include "controls.h"
#include "gamefield.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

signals:
    void sigCreateTrack();
    void sigSaveTrack(QString fileName);
    void sigLoadTrack(QString fileName);
    void sigGenetic(Params params);
    void sigResetBest();
    void sigAutoControl(bool control);

    void sigLeftClick(QPointF point);
    void sigRightClick(QPointF point);
    void sigKeyPressed(Key key);

    void sigRender(Render render);
    void sigGameStatus(GameStatus gameStatus);
    void sigActions(int n);

private:
    Controls* _controls = nullptr;
    GameField* _gameField = nullptr;
    QWidget* _centralWidget = nullptr;
};
