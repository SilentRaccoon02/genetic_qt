#include <QDebug>
#include <QTimer>

#include <absl/log/check.h>
#include <grpc++/grpc++.h>

#include "rpcserverimpl.h"

RpcServerImpl::RpcServerImpl(QObject *parent)
    : QObject(parent)
{
}

RpcServerImpl::~RpcServerImpl()
{
    server_->Shutdown();

    bool ok;
    void *tag;

    while (cq_->Next(&tag, &ok))
        static_cast<CallData *>(tag)->proceed(false);

    cq_->Shutdown();
}

int RpcServerImpl::next()
{
    return _n++;
}

void RpcServerImpl::slotStart()
{
    grpc::ServerBuilder builder;
    builder.AddListeningPort("localhost:50052", grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    std::cout << "Server listening" << std::endl;

    (new CountScoreCallData(this, &service_, cq_.get()))->proceed(true);
    (new SaveBestCallData(this, &service_, cq_.get()))->proceed(true);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RpcServerImpl::slotHandle);
    timer->start(0);
}

void RpcServerImpl::slotHandle()
{
    bool ok;
    void *tag;
    auto status = cq_->AsyncNext(&tag, &ok, gpr_now(GPR_CLOCK_REALTIME));

    if (status == grpc::CompletionQueue::SHUTDOWN)
    {
    }
    else if (status == grpc::CompletionQueue::GOT_EVENT)
    {
        static_cast<CallData *>(tag)->proceed(true);
    }
}

CallData::CallData(QObject *parent)
    : QObject(parent)
{
}

CountScoreCallData::CountScoreCallData(RpcServerImpl *server,
                                       common::Cpp::AsyncService *service,
                                       grpc::ServerCompletionQueue *cq,
                                       QObject *parent)
    : CallData(parent)
    , _server(server)
    , service_(service)
    , cq_(cq)
    , responder_(&ctx_)
    , status_(CREATE)
{
    connect(this, &CountScoreCallData::sigCountScore, _server, &RpcServerImpl::sigCountScore);
    connect(_server, &RpcServerImpl::sigAddScore, this, &CountScoreCallData::slotAddScore);
    _n = _server->next();
}

void CountScoreCallData::proceed(bool next)
{
    if (status_ == CREATE)
    {
        status_ = PROCESS;
        service_->RequestCountScore(&ctx_, &request_, &responder_, cq_, cq_, this);
    }
    else if (status_ == PROCESS)
    {
        if (next)
            (new CountScoreCallData(_server, service_, cq_))->proceed(next);

        QVector<double> w;

        for (auto const &value : qAsConst(request_.w()))
            w.append(value);

        emit sigCountScore(_n, w);
    }
    else
    {
        CHECK_EQ(status_, FINISH);
        delete this;
    }
}

void CountScoreCallData::slotAddScore(int n, int score)
{
    if (n != _n)
        return;

    reply_.set_value(score);
    status_ = FINISH;
    responder_.Finish(reply_, grpc::Status::OK, this);
}

SaveBestCallData::SaveBestCallData(RpcServerImpl *server,
                                   common::Cpp::AsyncService *service,
                                   grpc::ServerCompletionQueue *cq,
                                   QObject *parent)
    : CallData(parent)
    , _server(server)
    , service_(service)
    , cq_(cq)
    , responder_(&ctx_)
    , status_(CREATE)
{
    connect(this, &SaveBestCallData::sigSaveBest, _server, &RpcServerImpl::sigSaveBest);
}

void SaveBestCallData::proceed(bool next)
{
    if (status_ == CREATE)
    {
        status_ = PROCESS;
        service_->RequestSaveBest(&ctx_, &request_, &responder_, cq_, cq_, this);
    }
    else if (status_ == PROCESS)
    {
        if (next)
            (new SaveBestCallData(_server, service_, cq_))->proceed(next);

        QVector<double> w;

        for (auto const &value : qAsConst(request_.w()))
            w.append(value);

        emit sigSaveBest(w);

        reply_.set_ok(true);
        status_ = FINISH;
        responder_.Finish(reply_, grpc::Status::OK, this);
    }
    else
    {
        CHECK_EQ(status_, FINISH);
        delete this;
    }
}
