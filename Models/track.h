#pragma once

#include <QObject>

#include "Common/structs.h"

class Track : public QObject
{
    Q_OBJECT

public:
    struct Segment
    {
        QLineF centralLine;
        QLineF outerLine;
        QLineF innerLine;
    };

    Track(QObject *parent = nullptr);

    QVector<Segment> segments(bool closed) const;
    QVector<Check> checks(bool closed) const;

    void addPoint(QPointF point);
    void clear();

    bool saveJson(QString fileName) const;
    bool loadJson(QString fileName);

private:
    QLineF offsetLine(QLineF line, float offset) const;
    QPointF offsetPoint(QLineF line, float offset) const;

private:
    QVector<QPointF> _points;
};
