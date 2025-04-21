#include "track.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#define R 40
#define SPACE 20

#define X "x"
#define Y "y"
#define POINTS "points"

Track::Track(QObject *parent)
    : QObject(parent)
{
}

QVector<Track::Segment> Track::segments(bool closed) const
{
    QVector<Segment> segments;

    for (int i = 1; i < _points.size(); ++i)
    {
        QPointF a = _points[i - 1];
        QPointF b = _points[i];

        QLineF outerLine = offsetLine(QLineF(a, b), -R);
        QLineF innerLine = offsetLine(QLineF(a, b), R);

        if (i > 1)
        {
            QPointF a = _points[i - 2];
            QPointF b = _points[i - 1];

            QLineF prevOuter = offsetLine(QLineF(a, b), -R);
            QLineF prevInner = offsetLine(QLineF(a, b), R);

            outerLine.setP1(prevOuter.p2());
            innerLine.setP1(prevInner.p2());
        }

        segments.append(Segment{QLineF(a, b), outerLine, innerLine});
    }

    auto size = _points.size();

    if (closed && size > 2)
    {
        QLineF firstOuter = offsetLine(QLineF(_points[0], _points[1]), -R);
        QLineF firstInner = offsetLine(QLineF(_points[0], _points[1]), R);

        QLineF lastOuter = offsetLine(QLineF(_points[size - 2], _points[size - 1]), -R);
        QLineF lastInner = offsetLine(QLineF(_points[size - 2], _points[size - 1]), R);

        segments.append(Segment{QLineF(_points[0], _points[size - 1]),
                                QLineF(lastOuter.p2(), firstOuter.p1()),
                                QLineF(lastInner.p2(), firstInner.p1())});
    }

    return segments;
}

QVector<Check> Track::checks(bool closed) const
{
    auto segments = this->segments(closed);
    QVector<Check> checks;

    for (int i = 0; i < segments.size(); ++i)
    {
        auto outerLine = segments[i].outerLine;
        auto innerLine = segments[i].innerLine;

        auto count = int(outerLine.length() / SPACE);
        auto innerSpace = int(innerLine.length() / count);

        for (int i = 0; i < count; ++i)
        {
            QPointF a = offsetPoint(outerLine, i * SPACE);
            QPointF b = offsetPoint(innerLine, i * innerSpace);

            checks.append({QLineF(a, b), false});
        }
    }

    return checks;
}

void Track::addPoint(QPointF point)
{
    _points.append(point);
}

void Track::clear()
{
    _points.clear();
}

bool Track::saveJson(QString fileName) const
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Track: Unable to access file";
        return false;
    }

    QJsonArray pointsArray;

    for (auto const &point : qAsConst(_points))
    {
        QJsonObject pointObject;
        pointObject.insert(X, point.x());
        pointObject.insert(Y, point.y());
        pointsArray << pointObject;
    }

    QJsonObject jsonObject;
    jsonObject.insert(POINTS, pointsArray);

    QJsonDocument jsonDocument(jsonObject);
    file.write(jsonDocument.toJson(QJsonDocument::Indented));

    return true;
}

bool Track::loadJson(QString fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Track: Unable to access file";
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
    auto pointsArray = jsonObject.value(POINTS).toArray();

    for (auto const &value : qAsConst(pointsArray))
    {
        auto object = value.toObject();
        _points << QPointF(object.value(X).toInt(), object.value(Y).toInt());
    }

    return true;
}

QLineF Track::offsetLine(QLineF line, float offset) const
{
    float dx = line.x2() - line.x1();
    float dy = line.y2() - line.y1();

    float length = line.length();
    float perpX = -dy / length;
    float perpY = dx / length;

    float ax = line.x1() + perpX * offset;
    float ay = line.y1() + perpY * offset;
    float bx = line.x2() + perpX * offset;
    float by = line.y2() + perpY * offset;

    return QLineF(ax, ay, bx, by);
}

QPointF Track::offsetPoint(QLineF line, float offset) const
{
    float dx = line.x2() - line.x1();
    float dy = line.y2() - line.y1();

    float lenght = line.length();
    float scale = offset / lenght;

    float x = line.x1() + dx * scale;
    float y = line.y1() + dy * scale;

    return QPointF(x, y);
}
