#include "controls.h"
#include "Common/defines.h"
#include "ui_controls.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

Controls::Controls(QWidget *parent)
    : QWidget(parent)
{
    _ui = new Ui::Controls();
    _ui->setupUi(this);

    connect(_ui->createTrack, &QPushButton::clicked, this, &Controls::slotCreateTrack);
    connect(_ui->saveTrack, &QPushButton::clicked, this, &Controls::slotSaveTrack);
    connect(_ui->loadTrack, &QPushButton::clicked, this, &Controls::slotLoadTrack);

    connect(_ui->checksCheck, &QCheckBox::stateChanged, this, &Controls::slotField);
    connect(_ui->navLinesCheck, &QCheckBox::stateChanged, this, &Controls::slotField);
    connect(_ui->navPointsCheck, &QCheckBox::stateChanged, this, &Controls::slotField);

    Field field;
    _ui->checksCheck->setChecked(field.showChecks);
    _ui->navLinesCheck->setChecked(field.showNavLines);
    _ui->navPointsCheck->setChecked(field.showNavPoints);

    connect(_ui->runGenetic, &QPushButton::clicked, this, &Controls::slotGenetic);
}

Controls::~Controls()
{
    delete _ui;
}

void Controls::slotGameStatus(GameStatus trackStatus)
{
    _gameStatus = trackStatus;
}

void Controls::slotCreateTrack()
{
    if (_gameStatus == GameStatus::Busy)
    {
        QMessageBox messageBox;
        messageBox.setText("Unable to create when busy");
        messageBox.exec();
        return;
    }

    if (_gameStatus != GameStatus::Empty)
    {
        QMessageBox changeBox;
        changeBox.setText("Following action will clear the active track");
        changeBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        changeBox.setDefaultButton(QMessageBox::Ok);
        int reply = changeBox.exec();

        if (reply == QMessageBox::Cancel)
            return;
    }

    emit sigCreateTrack();
}

void Controls::slotSaveTrack()
{
    if (_gameStatus != GameStatus::Ready)
    {
        QMessageBox messageBox;
        messageBox.setText("Unable to save unfinished track");
        messageBox.exec();
        return;
    }

    auto fileName = QFileDialog::getSaveFileName(this, "Save file", APP_DATA_PATH, "JSON (*.json)");
    emit sigSaveTrack(fileName);
}

void Controls::slotLoadTrack()
{
    if (_gameStatus == GameStatus::Busy)
    {
        QMessageBox messageBox;
        messageBox.setText("Unable to load when busy");
        messageBox.exec();
        return;
    }

    if (_gameStatus != GameStatus::Empty)
    {
        QMessageBox changeBox;
        changeBox.setText("Following action will clear the active track");
        changeBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        changeBox.setDefaultButton(QMessageBox::Ok);
        int reply = changeBox.exec();

        if (reply == QMessageBox::Cancel)
            return;
    }

    auto fileName = QFileDialog::getOpenFileName(this, "Open file", APP_DATA_PATH, "JSON (*.json)");
    emit sigLoadTrack(fileName);
}

void Controls::slotField()
{
    Field field;
    field.showChecks = _ui->checksCheck->isChecked();
    field.showNavLines = _ui->navLinesCheck->isChecked();
    field.showNavPoints = _ui->navPointsCheck->isChecked();

    emit sigField(field);
}

void Controls::slotGenetic()
{
    Params params;
    params.hidSize = _ui->hidSize->value();
    params.maxAct = _ui->maxAct->value();
    params.maxGen = _ui->maxGen->value();
    params.popSize = _ui->popSize->value();
    params.hofSize = _ui->hofSize->value();
    params.randSeed = _ui->randSeed->value();
    params.pCross = _ui->pCross->value();
    params.pMut = _ui->pMut->value();

    emit sigGenetic(params);
}
