#pragma once

#include <QObject>
#include <QTimer>

#include "Common/structs.h"
#include "Models/car.h"
#include "Models/neural.h"
#include "Models/track.h"

class GameImpl : public QObject
{
    Q_OBJECT
public:
    GameImpl(QObject *parent = nullptr);

signals:
    void sigRender(Render render);
    void sigStatus(GameStatus gameStatus);
    void sigScore(int n, int score);
    void sigActions(int n);

public slots:
    void slotStart();
    void slotUpdate();

    void slotAddPoint(QPointF point);
    void slotStartCreate();
    void slotEndCreate();

    void slotSaveTrack(QString fileName);
    void slotLoadTrack(QString fileName);

    void slotKeyPressed(Key key);
    void slotAutoControl(bool control);
    void slotCountScore(int n, int maxAct, int hidSize, QVector<double> w);
    void slotSetBest(int hidSize, QVector<double> w);
    void slotResetBest();

private:
    void control();
    void restore();
    void resetTimer(int msec);

private:
    GameStatus _status = GameStatus::Empty;
    ControlType _controlType = ControlType::Auto;

    QTimer *_timer = nullptr;
    Neural *_neural = nullptr;
    Track *_track = nullptr;
    Car *_car = nullptr;

    QVector<Track::Segment> _segments;
    QVector<Check> _checks;

    int _score = 0;
    int _maxAct = 0;
    int _actions = 0;
    int _n = 0;
};
