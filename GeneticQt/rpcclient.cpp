#include <QThread>

#include "rpcclient.h"
#include "rpcclientimpl.h"

RpcClient::RpcClient(QObject *parent)
    : QObject(parent)
{
    auto rpcClientImpl = new RpcClientImpl();
    auto rpcClientThread = new QThread();
    rpcClientThread->setObjectName("RpcClientThread");

    connect(rpcClientThread, &QThread::started, rpcClientImpl, &RpcClientImpl::slotStart);
    connect(this, &QObject::destroyed, rpcClientThread, &QThread::quit);
    connect(rpcClientThread, &QThread::finished, rpcClientImpl, &RpcClientImpl::deleteLater);
    connect(rpcClientThread, &QThread::finished, rpcClientThread, &QThread::deleteLater);

    connect(this, &RpcClient::sigGenetic, rpcClientImpl, &RpcClientImpl::slotGenetic);

    rpcClientImpl->moveToThread(rpcClientThread);
    rpcClientThread->start();
}
