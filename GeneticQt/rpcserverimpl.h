#pragma once

#include <QDebug>
#include <QObject>
#include <QVector>

#include "common.grpc.pb.h"

class RpcServerImpl;
class CallData : public QObject
{
    Q_OBJECT

public:
    CallData(RpcServerImpl *server, common::Cpp::AsyncService* service, grpc::ServerCompletionQueue* cq);
    void proceed(bool next);

signals:
    void sigCountScore(int n, QVector<double> w);

public slots:
    void slotAddScore(int n, int score);

private:
    RpcServerImpl *_server = nullptr;
    common::Cpp::AsyncService* service_ = nullptr;
    grpc::ServerCompletionQueue* cq_ = nullptr;

    grpc::ServerContext ctx_;
    common::Individual request_;
    common::Score reply_;
    grpc::ServerAsyncResponseWriter<common::Score> responder_;
    enum CallStatus
    {
        CREATE,
        PROCESS,
        FINISH
    };
    CallStatus status_;
    int _n = 0;
};

class RpcServerImpl : public QObject
{
    Q_OBJECT

public:
    RpcServerImpl(QObject* parent = nullptr);
    ~RpcServerImpl();
    int next();

signals:
    void sigCountScore(int n, QVector<double> w);
    void sigAddScore(int n, int score);

public slots:
    void slotStart();

private slots:
    void slotHandle();

private:
    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    common::Cpp::AsyncService service_;
    std::unique_ptr<grpc::Server> server_;
    int _n = 0;
};
