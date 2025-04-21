#pragma once

#include <QObject>

#include "Common/structs.h"

class RpcClient : public QObject
{
    Q_OBJECT

public:
    RpcClient(QObject *parent = nullptr);

signals:
    void sigGenetic(Params params);
};
