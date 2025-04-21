#pragma once

#include <QObject>

class RpcServer : public QObject
{
    Q_OBJECT

public:
    RpcServer(QObject *parent = nullptr);

signals:
    void sigCountScore(int n, QVector<double> w);
    void sigAddScore(int n, int score);
};
