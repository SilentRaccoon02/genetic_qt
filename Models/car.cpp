#include <QDebug>
#include <QLineF>
#include <QtMath>

#include "car.h"

#define WIDTH 20
#define HEIGHT 40

#define R 200
#define MAX_DIST (R + 10)

#define MAX_SPEED 8
#define MIN_SPEED 0

#define SPEED_DELTA 0.1
#define ROTATION_DELTA 0.1

Car::Car(QPointF center, float angle, QObject *parent)
    : QObject(parent)
    , _center(center)
    , _angle(angle)
{
}

void Car::update()
{
    float x = qCos(_angle - M_PI_2) * _speed;
    float y = qSin(_angle - M_PI_2) * _speed;

    _center = QPointF(_center.x() + x, _center.y() + y);
}

void Car::moveForward()
{
    _speed += SPEED_DELTA;

    if (_speed >= MAX_SPEED)
        _speed = MAX_SPEED;
}

void Car::moveBack()
{
    _speed -= SPEED_DELTA;

    if (_speed <= MIN_SPEED)
        _speed = MIN_SPEED;
}

void Car::turnRight()
{
    _angle += ROTATION_DELTA;
}

void Car::turnLeft()
{
    _angle -= ROTATION_DELTA;
}

QPointF Car::A() const
{
    float x = _center.x() - WIDTH / 2;
    float y = _center.y() - HEIGHT / 2;

    return rotate(_center, QPointF(x, y));
}

QPointF Car::B() const
{
    float x = _center.x() + WIDTH / 2;
    float y = _center.y() - HEIGHT / 2;

    return rotate(_center, QPointF(x, y));
}

QPointF Car::C() const
{
    float x = _center.x() + WIDTH / 2;
    float y = _center.y() + HEIGHT / 2;

    return rotate(_center, QPointF(x, y));
}

QPointF Car::D() const
{
    float x = _center.x() - WIDTH / 2;
    float y = _center.y() + HEIGHT / 2;

    return rotate(_center, QPointF(x, y));
}

QLineF Car::lineLeftA() const
{
    QPointF a(A());
    QPointF b(a.x() - R / 2, a.y());

    return QLineF(a, rotate(a, b));
}

QLineF Car::lineLeftD() const
{
    QPointF a(D());
    QPointF b(a.x() - R / 2, a.y());

    return QLineF(a, rotate(a, b));
}

QLineF Car::lineRightB() const
{
    QPointF a(B());
    QPointF b(a.x() + R / 2, a.y());

    return QLineF(a, rotate(a, b));
}

QLineF Car::lineRightC() const
{
    QPointF a(C());
    QPointF b(a.x() + R / 2, a.y());

    return QLineF(a, rotate(a, b));
}

QLineF Car::lineForwardA() const
{
    QPointF a(A());
    QPointF b(a.x(), a.y() - R);

    return QLineF(a, rotate(a, b));
}

QLineF Car::lineForwardB() const
{
    QPointF a(B());
    QPointF b(a.x(), a.y() - R);

    return QLineF(a, rotate(a, b));
}

QLineF Car::lineLeftForwardA() const
{
    QPointF a(A());

    float bx = a.x() + R * qCos(-3 * M_PI / 4);
    float by = a.y() + R * qSin(-3 * M_PI / 4);

    return QLineF(a, rotate(a, QPointF(bx, by)));
}

QLineF Car::lineRightForwardB() const
{
    QPointF a(B());

    float bx = a.x() + R * qCos(-M_PI / 4);
    float by = a.y() + R * qSin(-M_PI / 4);

    return QLineF(a, rotate(a, QPointF(bx, by)));
}

QVector<QPointF> Car::points() const
{
    return QVector<QPointF>{A(), B(), C(), D()};
}

QVector<QLineF> Car::lines() const
{
    return QVector<QLineF>{lineLeftA(),
                           lineLeftD(),
                           lineRightB(),
                           lineRightC(),
                           lineForwardA(),
                           lineForwardB(),
                           lineLeftForwardA(),
                           lineRightForwardB()};
}

QVector<Car::Nav> Car::nav(QVector<QLineF> trackLines) const
{
    QVector<Nav> nav;
    auto lines = this->lines();

    for (auto const &line : qAsConst(lines))
    {
        Nav lineNav{QPointF(-10, -10), MAX_DIST};

        for (auto const &trackLine : qAsConst(trackLines))
        {
            QPointF point;
            auto type = line.intersect(trackLine, &point);

            if (type != QLineF::BoundedIntersection)
                continue;

            float dist = this->dist(line.p1(), point);

            if (dist < lineNav.dist)
            {
                lineNav.point = point;
                lineNav.dist = dist;
            }
        }

        nav.append(lineNav);
    }

    return nav;
}

bool Car::death(QVector<QLineF> trackLines) const
{
    QVector<QLineF> lines;
    lines.append(QLineF(A(), B()));
    lines.append(QLineF(B(), C()));
    lines.append(QLineF(C(), D()));
    lines.append(QLineF(D(), A()));

    for (auto const &line : qAsConst(lines))
    {
        for (auto const &trackLine : qAsConst(trackLines))
        {
            QPointF point;
            auto type = line.intersect(trackLine, &point);

            if (type != QLineF::BoundedIntersection)
                continue;

            return true;
        }
    }

    return false;
}

int Car::check(QVector<Check> trackChecks) const
{
    QVector<QLineF> lines;
    lines.append(QLineF(A(), B()));
    lines.append(QLineF(B(), C()));
    lines.append(QLineF(C(), D()));
    lines.append(QLineF(D(), A()));

    for (auto const &line : qAsConst(lines))
    {
        for (int i = 0; i < trackChecks.size(); ++i)
        {
            if (trackChecks[i].checked)
                continue;

            QPointF point;
            auto type = line.intersect(trackChecks[i].line, &point);

            if (type != QLineF::BoundedIntersection)
                continue;

            return i;
        }
    }

    return -1;
}

float Car::speed() const
{
    return _speed / MAX_SPEED;
}

QPointF Car::rotate(QPointF center, QPointF point) const
{
    float sin = qSin(_angle);
    float cos = qCos(_angle);

    float x1 = point.x() - center.x();
    float y1 = point.y() - center.y();

    float x = center.x() + cos * x1 - sin * y1;
    float y = center.y() + sin * x1 + cos * y1;

    return QPointF(x, y);
}

float Car::dist(QPointF a, QPointF b) const
{
    return qSqrt(qPow(b.x() - a.x(), 2) + qPow(b.y() - a.y(), 2));
}
