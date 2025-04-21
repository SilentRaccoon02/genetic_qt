#include <QThread>

#include "rpcserver.h"
#include "rpcserverimpl.h"

RpcServer::RpcServer(QObject *parent)
    : QObject(parent)
{
    auto rpcServerImpl = new RpcServerImpl();
    auto rpcServerThread = new QThread();
    rpcServerThread->setObjectName("RpcServerThread");

    connect(rpcServerThread, &QThread::started, rpcServerImpl, &RpcServerImpl::slotStart);
    connect(this, &QObject::destroyed, rpcServerThread, &QThread::quit);
    connect(rpcServerThread, &QThread::finished, rpcServerImpl, &RpcServerImpl::deleteLater);
    connect(rpcServerThread, &QThread::finished, rpcServerThread, &QThread::deleteLater);

    connect(rpcServerImpl, &RpcServerImpl::sigCountScore, this, &RpcServer::sigCountScore);
    connect(this, &RpcServer::sigAddScore, rpcServerImpl, &RpcServerImpl::sigAddScore);

    rpcServerImpl->moveToThread(rpcServerThread);
    rpcServerThread->start();
}
