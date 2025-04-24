#include "mainwindow.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _centralWidget = new QWidget(this);
    _controls = new Controls(_centralWidget);
    _gameField = new GameField(_centralWidget);

    auto layout = new QHBoxLayout(_centralWidget);
    layout->addWidget(_controls);
    layout->addWidget(_gameField);
    _centralWidget->setLayout(layout);

    setCentralWidget(_centralWidget);

    connect(_controls, &Controls::sigCreateTrack, this, &MainWindow::sigCreateTrack);
    connect(_controls, &Controls::sigSaveTrack, this, &MainWindow::sigSaveTrack);
    connect(_controls, &Controls::sigLoadTrack, this, &MainWindow::sigLoadTrack);
    connect(_controls, &Controls::sigGenetic, this, &MainWindow::sigGenetic);
    connect(_controls, &Controls::sigResetBest, this, &MainWindow::sigResetBest);
    connect(_controls, &Controls::sigAutoControl, this, &MainWindow::sigAutoControl);

    connect(_controls, &Controls::sigField, _gameField, &GameField::slotField);

    connect(_gameField, &GameField::sigLeftClick, this, &MainWindow::sigLeftClick);
    connect(_gameField, &GameField::sigRightClick, this, &MainWindow::sigRightClick);
    connect(_gameField, &GameField::sigKeyPressed, this, &MainWindow::sigKeyPressed);

    connect(this, &MainWindow::sigGameStatus, _controls, &Controls::slotGameStatus);
    connect(this, &MainWindow::sigRender, _gameField, &GameField::slotRender);
    connect(this, &MainWindow::sigGameStatus, _gameField, &GameField::slotGameStatus);
}
