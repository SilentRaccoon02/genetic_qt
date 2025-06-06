#include <QDebug>

#include "Common/defines.h"
#include "geneticqt.h"

GeneticQt::GeneticQt(int argc, char *argv[])
    : QApplication(argc, argv)
{
    qRegisterMetaType<Render>("Render");
    qRegisterMetaType<Params>("Params");
    qRegisterMetaType<GameStatus>("GameStatus");
    qRegisterMetaType<ControlType>("ControlType");
    qRegisterMetaType<QVector<double>>("QVector<double>");
    qRegisterMetaType<Key>("Key");

    _rpcClient = new RpcClient();
    _rpcServer = new RpcServer();

    _mainWindow = new MainWindow();
    _mainWindow->show();

    _game = new Game();

    connect(_mainWindow, &MainWindow::sigCreateTrack, _game, &Game::sigStartCreate);
    connect(_mainWindow, &MainWindow::sigSaveTrack, _game, &Game::sigSaveTrack);
    connect(_mainWindow, &MainWindow::sigLoadTrack, _game, &Game::sigLoadTrack);

    connect(_mainWindow, &MainWindow::sigLeftClick, _game, &Game::sigAddPoint);
    connect(_mainWindow, &MainWindow::sigRightClick, _game, &Game::sigEndCreate);
    connect(_mainWindow, &MainWindow::sigKeyPressed, _game, &Game::sigKeyPressed);
    connect(_mainWindow, &MainWindow::sigResetBest, _game, &Game::sigResetBest);
    connect(_mainWindow, &MainWindow::sigAutoControl, _game, &Game::sigAutoControl);

    connect(_game, &Game::sigStatus, _mainWindow, &MainWindow::sigGameStatus);
    connect(_game, &Game::sigActions, _mainWindow, &MainWindow::sigActions);
    connect(_game, &Game::sigRender, _mainWindow, &MainWindow::sigRender);

    connect(_mainWindow, &MainWindow::sigGenetic, this, &GeneticQt::slotGenetic);
    connect(this, &GeneticQt::sigCountScore, _game, &Game::sigCountScore);
    connect(this, &GeneticQt::sigSetBest, _game, &Game::sigSetBest);

    connect(_game, &Game::sigScore, this, &GeneticQt::slotScore);

    connect(this, &GeneticQt::sigGenetic, _rpcClient, &RpcClient::sigGenetic);

    connect(_rpcServer, &RpcServer::sigCountScore, this, &GeneticQt::slotCountScore);
    connect(_rpcServer, &RpcServer::sigSaveBest, this, &GeneticQt::slotSetBest);


    connect(this, &GeneticQt::sigAddScore, _rpcServer, &RpcServer::sigAddScore);

}

GeneticQt::~GeneticQt()
{
    delete _rpcClient;
    delete _rpcServer;
    delete _mainWindow;
    delete _game;
}

void GeneticQt::slotCountScore(int n, QVector<double> w)
{
    int maxAct = _params.maxAct;

    if (n > _params.genSwitch * _params.popSize)
        maxAct = _params.actSwitch;

    emit sigCountScore(n, maxAct, _params.hidSize, w);
}

void GeneticQt::slotSetBest(QVector<double> w)
{
    emit sigSetBest(_params.hidSize, w);
}

void GeneticQt::slotScore(int n, int score)
{
    emit sigAddScore(n, score);
}

void GeneticQt::slotGenetic(Params params)
{
    params.wSize = wSize(params.hidSize);
    _params = params;
    emit sigGenetic(params);
}

int GeneticQt::wSize(int hidSize)
{
    int inHidSize = IN_SIZE * hidSize + hidSize;
    int hidOutSize = hidSize * OUT_SIZE + OUT_SIZE;

    return inHidSize + hidOutSize;
}
