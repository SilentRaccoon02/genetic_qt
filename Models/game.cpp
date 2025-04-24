#include <QThread>

#include "game.h"
#include "gameimpl.h"

Game::Game(QObject *parent)
    : QObject(parent)
{
    auto gameImpl = new GameImpl();
    auto gameThread = new QThread();
    gameThread->setObjectName("GameImplThread");

    connect(gameThread, &QThread::started, gameImpl, &GameImpl::slotStart);
    connect(this, &QObject::destroyed, gameThread, &QThread::quit);
    connect(gameThread, &QThread::finished, gameImpl, &GameImpl::deleteLater);
    connect(gameThread, &QThread::finished, gameThread, &QThread::deleteLater);

    connect(gameImpl, &GameImpl::sigRender, this, &Game::sigRender);
    connect(gameImpl, &GameImpl::sigStatus, this, &Game::sigStatus);
    connect(gameImpl, &GameImpl::sigScore, this, &Game::sigScore);

    connect(this, &Game::sigAddPoint, gameImpl, &GameImpl::slotAddPoint);
    connect(this, &Game::sigStartCreate, gameImpl, &GameImpl::slotStartCreate);
    connect(this, &Game::sigEndCreate, gameImpl, &GameImpl::slotEndCreate);
    connect(this, &Game::sigSaveTrack, gameImpl, &GameImpl::slotSaveTrack);
    connect(this, &Game::sigLoadTrack, gameImpl, &GameImpl::slotLoadTrack);
    connect(this, &Game::sigKeyPressed, gameImpl, &GameImpl::slotKeyPressed);

    connect(this, &Game::sigAutoControl, gameImpl, &GameImpl::slotAutoControl);
    connect(this, &Game::sigCountScore, gameImpl, &GameImpl::slotCountScore);
    connect(this, &Game::sigSetBest, gameImpl, &GameImpl::slotSetBest);
    connect(this, &Game::sigResetBest, gameImpl, &GameImpl::slotResetBest);

    gameImpl->moveToThread(gameThread);
    gameThread->start();
}
