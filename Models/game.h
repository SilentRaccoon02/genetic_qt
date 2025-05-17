#pragma once

#include <QObject>

#include "Common/structs.h"

class Game : public QObject
{
    Q_OBJECT

public:
    Game(QObject *parent = nullptr);
    void update();

signals:
    void sigRender(Render render);
    void sigStatus(GameStatus gameStatus);
    void sigScore(int n, int score);
    void sigActions(int n);

    void sigAddPoint(QPointF point);
    void sigStartCreate();
    void sigEndCreate();

    void sigSaveTrack(QString fileName);
    void sigLoadTrack(QString fileName);
    void sigKeyPressed(Key key);

    void sigAutoControl(bool control);
    void sigCountScore(int n, int maxAct, int hidSize, QVector<double> w);
    void sigSetBest(int hidSize, QVector<double> w);
    void sigResetBest();
};
