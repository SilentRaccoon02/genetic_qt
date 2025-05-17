#pragma once

#include <QWidget>

#include "Common/structs.h"

namespace Ui {
class Controls;
}

class Controls : public QWidget
{
    Q_OBJECT

public:
    Controls(QWidget *parent = nullptr);
    ~Controls();

signals:
    void sigCreateTrack();
    void sigSaveTrack(QString fileName);
    void sigLoadTrack(QString fileName);
    void sigField(Field field);
    void sigGenetic(Params params);
    void sigResetBest();
    void sigAutoControl(bool control);

public slots:
    void slotGameStatus(GameStatus gameStatus);
    void slotActions(int n);

private slots:
    void slotCreateTrack();
    void slotSaveTrack();
    void slotLoadTrack();
    void slotField();
    void slotGenetic();
    void slotAutoControl();

private:
    void addTrack();

private:
    Ui::Controls *_ui;
    GameStatus _gameStatus = GameStatus::Empty;
};
