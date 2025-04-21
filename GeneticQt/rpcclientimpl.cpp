#include <QDebug>
#include <grpc++/grpc++.h>

#include "common.pb.h"
#include "rpcclientimpl.h"

RpcClientImpl::RpcClientImpl(QObject *parent)
    : QObject(parent)
{
}

void RpcClientImpl::slotStart()
{
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    _stub = common::Python::NewStub(channel);
}

void RpcClientImpl::slotGenetic(Params params)
{
    grpc::ClientContext context;
    common::Params request;
    common::Status reply;

    request.set_wsize(params.wSize);
    request.set_hidsize(params.hidSize);
    request.set_maxact(params.maxAct);
    request.set_maxgen(params.maxGen);
    request.set_popsize(params.popSize);
    request.set_hofsize(params.hofSize);
    request.set_randseed(params.randSeed);
    request.set_pcross(params.pCross);
    request.set_pmut(params.pMut);

    grpc::Status status = _stub->Genetic(&context, request, &reply);

    if (!status.ok())
        std::cout << "Error: " << status.error_message() << std::endl;
}
