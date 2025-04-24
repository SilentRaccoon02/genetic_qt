#pragma once

#include <QLineF>
#include <QPointF>
#include <QVector>

enum GameStatus
{
    Empty,
    Collects,
    Ready,
    Busy
};

enum ControlType
{
    Manual,
    Auto
};

enum Key
{
    W,
    A,
    S,
    D
};

struct Render
{
    QVector<QLineF> trackLines;
    QVector<QLineF> trackChecks;
    QVector<QPointF> carPoints;
    QVector<QPointF> carNavPoints;
    QVector<QLineF> carNavLines;
};

struct Check
{
    QLineF line;
    bool checked;
};

struct Params
{
    int wSize;
    int hidSize;
    int maxAct;
    int maxGen;
    int popSize;
    int hofSize;
    int randSeed;
    float pCross;
    float pMut;
};

struct Field {
    bool showChecks = false;
    bool showNavLines = false;
    bool showNavPoints = true;
};
