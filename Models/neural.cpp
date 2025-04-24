#include "neural.h"
#include "Common/defines.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#define W QStringLiteral("w")
#define HID_SIZE QStringLiteral("hid_size")

Neural::Neural(QObject *parent)
    : QObject(parent)

{
}

QVector<double> Neural::norm(QVector<double> in)
{
    QVector<double> out(in.size());

    double min = *std::min_element(in.begin(), in.end());
    double max = *std::max_element(in.begin(), in.end());

    for (int i = 0; i < out.size(); ++i)
        out[i] = (in[i] - min) / (max - min);

    return out;
}

QVector<int> Neural::split(QVector<double> in)
{
    auto softmax1 = softmax(QVector<double>{in[0], in[1], in[2]});
    auto softmax2 = softmax(QVector<double>{in[3], in[4], in[5]});

    double max1 = -1;
    double max2 = -1;

    int pred1 = -1;
    int pred2 = -1;

    for (int i = 0; i < 3; ++i)
    {
        if (softmax1[i] > max1)
        {
            max1 = softmax1[i];
            pred1 = i;
        }

        if (softmax2[i] > max2)
        {
            max2 = softmax2[i];
            pred2 = i;
        }
    }

    return QVector<int>{pred1, pred2};
}

double Neural::linear(double x)
{
    return x;
}

double Neural::relu(double x)
{
    return x > 0 ? x : 0;
}

QVector<double> Neural::softmax(QVector<double> in)
{
    QVector<double> out;

    double max = *std::max_element(in.begin(), in.end());
    double sum = 0;

    for (double val : in)
        sum += std::exp(val - max);

    for (double val : in)
        out.append(std::exp(val - max) / sum);

    return out;
}

void Neural::resize(int inSize, int hidSize, int outSize)
{
    _inSize = inSize;
    _hidSize = hidSize;
    _outSize = outSize;
}

void Neural::set(QVector<double> w)
{
    int n = 0;
    _w = w;

    _hidIn.resize(_hidSize);

    for (int i = 0; i < _hidSize; ++i)
    {
        _hidIn[i].resize(_inSize + 1);

        for (int j = 0; j < _inSize + 1; ++j)
        {
            _hidIn[i][j] = w[n];
            ++n;
        }
    }

    _outHid.resize(_outSize);

    for (int i = 0; i < _outSize; ++i)
    {
        _outHid[i].resize(_hidSize + 1);

        for (int j = 0; j < _hidSize + 1; ++j)
        {
            _outHid[i][j] = w[n];
            ++n;
        }
    }

    _ready = true;
}

QVector<double> Neural::predict(QVector<double> in) const
{
    if (!_ready)
        return QVector<double>();

    QVector<double> hid(_hidSize);
    QVector<double> out(_outSize);

    in.append(1.0); // bias

    for (int i = 0; i < _hidSize; ++i)
    {
        double sum = 0;

        for (int j = 0; j < _inSize + 1; ++j)
            sum += in[j] * _hidIn[i][j];

        hid[i] = relu(sum);
    }

    hid.append(1.0); // bias

    for (int i = 0; i < _outSize; ++i)
    {
        double sum = 0;

        for (int j = 0; j < _hidSize + 1; ++j)
            sum += hid[j] * _outHid[i][j];

        out[i] = linear(sum);
    }

    return out;
}

bool Neural::saveJson(QString fileName) const
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Neural: Unable to access file";
        return false;
    }

    QJsonArray wArray;

    for (auto const &value : qAsConst(_w))
        wArray.append(value);

    QJsonObject jsonObject;
    jsonObject.insert(W, wArray);
    jsonObject.insert(HID_SIZE, _hidSize);

    QJsonDocument jsonDocument(jsonObject);
    file.write(jsonDocument.toJson(QJsonDocument::Indented));

    return true;
}

bool Neural::loadJson(QString fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Neural: Unable to access file";
        return false;
    }

    QJsonParseError parseError;
    auto jsonDocument = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        qDebug() << "Parse error:" << parseError.errorString();
        return false;
    }

    auto jsonObject = jsonDocument.object();
    auto wArray = jsonObject.value(W).toArray();
    int hidSize = jsonObject.value(HID_SIZE).toInt();

    QVector<double> w;

    for (auto const &value : qAsConst(wArray))
        w.append(value.isDouble());

    resize(IN_SIZE, hidSize, OUT_SIZE);
    set(w);

    return true;
}

QVector<double> Neural::w() const
{
    return _w;
}
