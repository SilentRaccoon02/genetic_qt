#pragma once

#include <QObject>
#include <memory>

#include "Common/structs.h"
#include "common.grpc.pb.h"

class RpcClientImpl : public QObject
{
    Q_OBJECT

public:
    RpcClientImpl(QObject *parent = nullptr);

public slots:
    void slotStart();
    void slotGenetic(Params params);

private:
    std::unique_ptr<common::Python::Stub> _stub;
};
