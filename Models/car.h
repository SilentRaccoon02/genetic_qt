#pragma once

#include <QObject>

#include "Common/structs.h"

class Car : public QObject
{
public:
    struct Nav
    {
        QPointF point;
        float dist;
    };

    Car(QPointF center, float angle, QObject* parent = nullptr);
    void update();

    QPointF A() const;
    QPointF B() const;
    QPointF C() const;
    QPointF D() const;

    QLineF lineLeftA() const;
    QLineF lineLeftD() const;
    QLineF lineRightB() const;
    QLineF lineRightC() const;
    QLineF lineForwardA() const;
    QLineF lineForwardB() const;
    QLineF lineLeftForwardA() const;
    QLineF lineRightForwardB() const;

    QVector<QPointF> points() const;
    QVector<QLineF> lines() const;
    QVector<Nav> nav(QVector<QLineF> trackLines) const;
    bool death(QVector<QLineF> trackLines) const;
    int check(QVector<Check> trackChecks) const;
    float speed() const;

    void moveForward();
    void moveBack();
    void turnRight();
    void turnLeft();

private:
    QPointF rotate(QPointF center, QPointF point) const;
    float dist(QPointF a, QPointF b) const;

private:
    QPointF _center;
    float _angle = 0;
    float _speed = 0;
    int _score = 0;
};
