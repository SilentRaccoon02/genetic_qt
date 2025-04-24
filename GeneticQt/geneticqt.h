#pragma once

#include <QApplication>

#include "Views/mainwindow.h"
#include "Models/game.h"
#include "rpcclient.h"
#include "rpcserver.h"

class GeneticQt : public QApplication
{
    Q_OBJECT

public:
    GeneticQt(int argc, char *argv[]);
    ~GeneticQt();

signals:
    void sigCountScore(int n, int maxAct, int hidSize, QVector<double> w);
    void sigSetBest(int hidSize, QVector<double> w);

    void sigGenetic(Params params);
    void sigAddScore(int n, int score);

public slots:
    void slotCountScore(int n, QVector<double> w);
    void slotSetBest(QVector<double> w);

    void slotScore(int n, int score);
    void slotGenetic(Params params);

private:
    int wSize(int hidSize);

private:
    RpcClient *_rpcClient = nullptr;
    RpcServer* _rpcServer = nullptr;

    MainWindow *_mainWindow = nullptr;
    Game *_game = nullptr;

    Params _params;
};
