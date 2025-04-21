#pragma once

#include <QObject>
#include <QVector>

class Neural : public QObject
{
    Q_OBJECT

public:
    Neural(QObject *parent = nullptr);

    static QVector<double> norm(QVector<double> in);
    static QVector<int> split(QVector<double> in);

    static inline double linear(double x);
    static inline double relu(double x);
    static QVector<double> softmax(QVector<double> in);

    void resize(int inSize, int hidSize, int outSize);
    void set(QVector<double> w);
    QVector<double> predict(QVector<double> in) const;

private:
    int _inSize = 0;
    int _hidSize = 0;
    int _outSize = 0;

    QVector<QVector<double>> _hidIn;
    QVector<QVector<double>> _outHid;
};
