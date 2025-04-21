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

public slots:
    void slotGameStatus(GameStatus gameStatus);

private slots:
    void slotCreateTrack();
    void slotSaveTrack();
    void slotLoadTrack();
    void slotField();
    void slotGenetic();

private:
    void addTrack();

private:
    Ui::Controls *_ui;
    GameStatus _gameStatus = GameStatus::Empty;
};
