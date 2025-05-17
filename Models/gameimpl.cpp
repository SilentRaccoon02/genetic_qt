#include <QDebug>
#include <QtMath>

#include "Common/defines.h"
#include "gameimpl.h"

GameImpl::GameImpl(QObject *parent)
    : QObject(parent)
{
}

void GameImpl::slotStart()
{
    _neural = new Neural(this);
    _track = new Track(this);
    slotLoadTrack(DEFAULT_TRACK_PATH);
    resetTimer(10);
}

void GameImpl::slotUpdate()
{
    Render render;
    QVector<QLineF> trackLines;
    QVector<QLineF> trackChecks;

    if (_status == GameStatus::Ready || _status == GameStatus::Busy)
    {
        for (auto const &segment : qAsConst(_segments))
        {
            trackLines.append(segment.innerLine);
            trackLines.append(segment.outerLine);
        }

        for (auto const &check : qAsConst(_checks))
            if (!check.checked)
                trackChecks.append(check.line);

        control();

        auto carNav = _car->nav(trackLines);
        QVector<QPointF> carNavPoints;

        for (auto const carNavPoint : qAsConst(carNav))
            carNavPoints.append(carNavPoint.point);

        render.carPoints = _car->points();
        render.carNavPoints = carNavPoints;
        render.carNavLines = _car->lines();
    }
    else
    {
        auto segments = _track->segments(false);
        auto checks = _track->checks(false);

        for (auto const &segment : qAsConst(segments))
        {
            trackLines.append(segment.centralLine);
            trackLines.append(segment.innerLine);
            trackLines.append(segment.outerLine);
        }

        for (auto const &check : qAsConst(checks))
            trackChecks.append(check.line);
    }

    render.trackLines = trackLines;
    render.trackChecks = trackChecks;

    emit sigRender(render);
}

void GameImpl::slotAddPoint(QPointF point)
{
    if (_status == GameStatus::Collects)
        _track->addPoint(point);
}

void GameImpl::slotStartCreate()
{
    if (_status == GameStatus::Busy)
    {
        qDebug() << "GameImpl: Unable to create when busy";
        return;
    }

    _track->clear();

    _status = GameStatus::Collects;
    emit sigStatus(_status);
}

void GameImpl::slotEndCreate()
{
    restore();

    _status = GameStatus::Ready;
    emit sigStatus(_status);
}

void GameImpl::slotSaveTrack(QString fileName)
{
    qDebug() << fileName;

    if (_status != GameStatus::Ready)
    {
        qDebug() << "GameImpl: Unable to save unfinished track";
        return;
    }

    _track->saveJson(fileName);
}

void GameImpl::slotLoadTrack(QString fileName)
{
    if (_status == GameStatus::Busy)
    {
        qDebug() << "GameImpl: Unable to load when busy";
        return;
    }

    _track->clear();

    if (!_track->loadJson(fileName))
        return;

    restore();

    _status = GameStatus::Ready;
    emit sigStatus(_status);
}

void GameImpl::slotKeyPressed(Key key)
{
    if (!_car)
        return;

    if (key == Key::W)
        _car->moveForward();

    if (key == Key::A)
        _car->turnLeft();

    if (key == Key::S)
        _car->moveBack();

    if (key == Key::D)
        _car->turnRight();
}

void GameImpl::slotAutoControl(bool control)
{
    qDebug() << control;

    if (_status == GameStatus::Ready)
    {
        if (control)
            _controlType = Auto;
        else
            _controlType = Manual;
    }
}

void GameImpl::slotCountScore(int n, int maxAct, int hidSize, QVector<double> w)
{
    restore();
    _status = GameStatus::Busy;
    emit sigStatus(_status);

    _n = n;
    _maxAct = maxAct;
    _neural->resize(IN_SIZE, hidSize, OUT_SIZE);
    _neural->set(w);

    resetTimer(0);
}

void GameImpl::slotSetBest(int hidSize, QVector<double> w)
{
    restore();
    _status = GameStatus::Ready;
    emit sigStatus(_status);

    _neural->resize(IN_SIZE, hidSize, OUT_SIZE);
    _neural->set(w);

    resetTimer(16);
    //qDebug() << "SET BEST" << w;
}

void GameImpl::slotResetBest()
{
    restore();
}

void GameImpl::control()
{
    // --- data receiving ---
    _car->update();

    QVector<QLineF> trackLines;

    for (auto const &segment : qAsConst(_segments))
    {
        trackLines.append(segment.innerLine);
        trackLines.append(segment.outerLine);
    }

    bool death = _car->death(trackLines);
    int check = _car->check(_checks);

    if (check != -1)
    {
        if (!check || _checks[check - 1].checked)
        {
            _checks[check].checked = true;

            if (_status == GameStatus::Busy)
                ++_score;
        }

        if (_checks[_checks.size() - 2].checked && check == _checks.size() - 1)
            for (int i = 0; i < _checks.size(); ++i)
                _checks[i].checked = false;
    }

    // --- score calculation ---

    if (_status == GameStatus::Busy)
    {
        --_maxAct;

        if (death)
            _score = -10;

        if (death || !_maxAct)
        {
            _actions = 0;
            // auto w = _neural->w();
            // qDebug() << "maxAct" << _maxAct << "score" <<_score << "first_w" << w[0] << "n" <<_n;

            _status = GameStatus::Ready;
            emit sigStatus(_status);
            emit sigScore(_n, _score);
            return;
        }
    }

    // --- control ---

    if (_status != GameStatus::Busy && _controlType != ControlType::Auto)
        return;

    auto carNav = _car->nav(trackLines);
    QVector<double> carNavDists;

    for (auto const carNavPoint : qAsConst(carNav))
        carNavDists.append(carNavPoint.dist);

    auto in = Neural::norm(carNavDists);
    in.append(_car->speed());

    QVector<double> out = _neural->predict(in);

    if (out.size() != OUT_SIZE)
        return;

    auto pred = Neural::split(out);

    if (pred[0] == 1)
        _car->turnLeft();

    if (pred[0] == 2)
        _car->turnRight();

    if (pred[1] == 1)
        _car->moveForward();

    if (pred[1] == 2)
        _car->moveBack();

    _actions++;
    emit sigActions(_actions);
}

void GameImpl::restore()
{
    _score = 0;
    _maxAct = 0;
    _n = 0;

    _actions = 0;
    emit sigActions(_actions);

    _segments = _track->segments(true);
    _checks = _track->checks(true);

    auto start = _segments[0].centralLine.p1();
    auto angle = -_segments[0].centralLine.angle() * M_PI / 180 + M_PI_2;

    _car = new Car(start, angle, this);
}

void GameImpl::resetTimer(int msec)
{
    if (_timer)
        delete _timer;

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &GameImpl::slotUpdate);
    _timer->start(msec);
}
