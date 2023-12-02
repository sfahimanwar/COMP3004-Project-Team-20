#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include<unistd.h>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->aedDisplay->setPixmap(QPixmap(":/resources/img/aed.jpg"));

    //Can Set either Patient or Child Image using the code below
    /*
    ui->patientLabel->setPixmap(QPixmap(":/resources/img/child.jpg"));
    ui->patientLabel->setPixmap(QPixmap(":/resources/img/dummy.jpg"));
    */

    //Can set ECG image using the code below
    /*
    ui->ecgLabel->setPixmap(QPixmap(":/resources/img/normal.jpg"));
    ui->ecgLabel->setPixmap(QPixmap(":/resources/img/fast.jpg"));
    ui->ecgLabel->setPixmap(QPixmap(":/resources/img/slow.jpg"));
    ui->ecgLabel->setPixmap(QPixmap(":/resources/img/irregular.jpg"));
    */

    this->aed = new AED();

    QPushButton* userButton;
    userButton = MainWindow::findChild<QPushButton *>("attachDefibButton");
    connect(userButton, SIGNAL(released()), this, SLOT(applyPads()));
    userButton->setDisabled(true);

    userButton = MainWindow::findChild<QPushButton *>("helpButton");
    connect(userButton, SIGNAL(released()), this, SLOT(callEMS()));
    userButton->setDisabled(true);

    userButton = MainWindow::findChild<QPushButton *>("moveBackButton");
    connect(userButton, SIGNAL(released()), this, SLOT(moveAway()));
    userButton->setDisabled(true);

    userButton = MainWindow::findChild<QPushButton *>("openChestButton");
    connect(userButton, SIGNAL(released()), this, SLOT(clearChest()));
    userButton->setDisabled(true);

    userButton = MainWindow::findChild<QPushButton *>("powerButton");
    connect(userButton, SIGNAL(released()), this, SLOT(powerOn()));

    userButton = MainWindow::findChild<QPushButton *>("responsivenessButton");
    connect(userButton, SIGNAL(released()), this, SLOT(checkResponse()));
    userButton->setDisabled(true);

    userButton = MainWindow::findChild<QPushButton *>("openAirwaysButton");
    connect(userButton, SIGNAL(released()), this, SLOT(openAirways()));
    userButton->setDisabled(true);

    userButton = MainWindow::findChild<QPushButton *>("checkBreathingButton");
    connect(userButton, SIGNAL(released()), this, SLOT(checkBreathing()));
    userButton->setDisabled(true);

    userButton = MainWindow::findChild<QPushButton *>("shockButton");
    connect(userButton, SIGNAL(released()), this, SLOT(shock()));
    userButton->setDisabled(true);

    userButton = MainWindow::findChild<QPushButton *>("createPatientButton");
    connect(userButton, SIGNAL(released()), this, SLOT(beginSimulation()));

    ui->conditionBox->addItem("No Issue");
    ui->conditionBox->addItem("VF");
    ui->conditionBox->addItem("VT");
    ui->conditionBox->addItem("Not Shockable");


    //TODO: Connect CPR buttons

    updateTextbox("To begin using the AED, press the power button.");

    ui->userActionsFrame->setDisabled(true);
    ui->aedAudioFrame->setDisabled(true);
    ui->cprFrame->setDisabled(true);


//    series = new QLineSeries();
//    chart = new QChart();
//    //VT
//    for (int i = 0; i < 1000; i = i + 40) {
//        series->append(i + 0,50);
//        series->append(i + 5,72.5);
//        series->append(i + 10,75);
//        series->append(i + 15,72.5);
//        series->append(i + 20, 50);
//        series->append(i + 25, 27.5);
//        series->append(i + 30, 25);
//        series->append(i + 35, 27.5);
//    }
////    series->append(0, 6);
////    series->append(2, 4);
////    series->append(3, 8);
////    series->append(7, 4);
////    series->append(10, 5);
//    //*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
//    chart->legend()->hide();
//    chart->addSeries(series);
//    QValueAxis *xAxis = new QValueAxis;
//    xAxis->setLabelFormat("%.0f");
//    xAxis->setRange(0, 200);
//    xAxis->setTitleText("Time (s)");
//    QValueAxis *yAxis = new QValueAxis;
//    yAxis->setLabelFormat("%.0f");
//    yAxis->setRange(0, 100);
//    yAxis->setTitleText("Heart Rate (bpm)");
//    yAxis->setTickCount(6);
//    chart->addAxis(xAxis, Qt::AlignBottom);
//    chart->addAxis(yAxis, Qt::AlignLeft);
//    series->attachAxis(xAxis);
//    series->attachAxis(yAxis);
//    chartView = new QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);
//    chartView->resize(ui->horizontalFrame->size());
//    chartView->setParent(ui->horizontalFrame);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeGraph(){
//    series = new QLineSeries();
//    chart = new QChart();
//    chart->legend()->hide();
//    chart->addSeries(series);
//    QValueAxis *xAxis = new QValueAxis;
//    xAxis->setLabelFormat("%.0f");
//    xAxis->setRange(0, 60);
//    xAxis->setTitleText("Time (s)");
//    QValueAxis *yAxis = new QValueAxis;
//    yAxis->setLabelFormat("%.0f");
//    yAxis->setRange(70, 120);
//    yAxis->setTitleText("Heart Rate (bpm)");
//    yAxis->setTickCount(6);
//    chart->addAxis(xAxis, Qt::AlignBottom);
//    chart->addAxis(yAxis, Qt::AlignLeft);
//    series->attachAxis(xAxis);
//    series->attachAxis(yAxis);
//    chartView = new QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);
//    chartView->resize(ui->horizontalFrame->size());
//    chartView->setParent(ui->horizontalFrame);
}








void MainWindow::beginSimulation(){

    // Get values from buttons to create patient and begin simulation
    int body = 0;
    int condition = -1;
    int pulse = ui->pulseSetBox->value();
    bool pulseSafeRange = false;
    bool regPulse = false;
    bool response = false;
    bool breathing = false;

    if (ui->bodyBox->currentText() == "Child"){
        body = 1;
    }

    if (ui->pulseEvenBox->currentText() == "T"){
        regPulse = true;
    }

    if(60 < pulse && pulse < 100){
        pulseSafeRange = true;
    }

    if(ui->responsivenessBox->currentText() == "T"){
        response = true;
    }

    if(ui->isBreathingBox->currentText() == "T"){
        breathing = true;
    }
    if (ui->conditionBox->currentText() == "VF"){
        condition = 0;
    }
    else if (ui->conditionBox->currentText() == "VT"){
        condition = 1;
    }
    else if (ui->conditionBox->currentText() == "Not Shockable"){
        condition = 2;
    } else {
        condition = 3;
    }

    patient= new Patient(body, pulse, regPulse, pulseSafeRange, response, breathing, condition);

    ui->userActionsFrame->setEnabled(true);
    ui->aedAudioFrame->setEnabled(true);

    qDebug() << "Created a patient with values body:" << body << "pulse:" << pulse << " pulseSafeRange" << pulseSafeRange << "regPulse:" << regPulse << "response:" << response << "breathing:" << breathing << "condition" << condition;
    ui->configFrame->setDisabled(true);
}

void MainWindow::updateTextbox(QString message){
    ui->aedText->append(message);
}

void MainWindow::powerOn(){
    updateTextbox("The AED has been powered on!");
    updateTextbox("Check the responsiveness of the patient.");

    ui->powerButton->setDisabled(true);
    ui->responsivenessButton->setEnabled(true);

    ui->okState->setChecked(true);
}

void MainWindow::checkResponse(){
    bool response = patient->getResponsive();
    if (response){
        updateTextbox("The patient is responsive--do NOT use the AED! Wait for help to arrive!");
        //TODO: stuff here
    }else{
        updateTextbox("The patient is unresponsive. Call for help and prepare to administer CPR.");

        ui->responsivenessButton->setDisabled(true);
        ui->helpButton->setEnabled(true);

        ui->okState->setChecked(false);
        ui->emergencyState->setChecked(true);
    }

}

void MainWindow::callEMS(){
    updateTextbox("EMS has been called, and will be arriving shortly!");
    EMSTimer = 4;   //TODO: Make this random in a limited range
    qDebug() << "EMSTimer has been set to" << EMSTimer;

    updateTextbox("Open the patient's airways.");

    ui->helpButton->setDisabled(true);
    ui->openAirwaysButton->setEnabled(true);

    ui->emergencyState->setChecked(false);
    ui->airwayState->setChecked(true);
}

void MainWindow::openAirways(){
    updateTextbox("The patient's airways have been opened, check if the patient is breathing.");

    ui->openAirwaysButton->setDisabled(true);
    ui->checkBreathingButton->setEnabled(true);

    ui->airwayState->setChecked(false);
    ui->breathState->setChecked(true);

}

void MainWindow::checkBreathing(){
    if(patient->getBreathing() == false){
        updateTextbox("The patient is not breathing. Prepare to apply defibrilator pads. First, clear the patient's chest.");
    }else{
        updateTextbox("The patient is breathing. TODO: SOMETHING HERE");
    }
    ui->checkBreathingButton->setDisabled(true);
    ui->openChestButton->setEnabled(true);

    ui->breathState->setChecked(false);

}

void MainWindow::clearChest(){
    updateTextbox("Patient's chest has been cleared.");
    updateTextbox("Attach the patient's defibrillator pads now.");

    ui->rightElectrode->setEnabled(true);
    ui->leftElectrode->setEnabled(true);

    ui->openChestButton->setDisabled(true);
    ui->attachDefibButton->setEnabled(true);

    ui->breathState->setChecked(false);
    ui->padState->setChecked(true);

}

void MainWindow::applyPads(){
    // TODO: Display ECG reading once the pads are attached
    bool electrode1 = ui->leftElectrode->isChecked();
    bool electrode2 = ui->rightElectrode->isChecked();
    if (electrode1 && electrode2){
        updateTextbox("Both defibrillator pads are in now place. Move back, do not touch the patient!");
        ui->rightElectrode->setDisabled(true);
        ui->leftElectrode->setDisabled(true);

        ui->attachDefibButton->setDisabled(true);
        ui->moveBackButton->setEnabled(true);

        ui->padState->setChecked(false);
        ui->noTouchState->setChecked(true);


    }else{
        updateTextbox("Defibrillator pads not attached!");
    }
}

void MainWindow::moveAway(){
    isTouching = false;
    updateTextbox("Prepare to administer CPR [CPR INSTRUCTIONS HERE]");

    ui->moveBackButton->setDisabled(true);
    ui->cprFrame->setEnabled(true);

    ui->noTouchState->setChecked(false);
    ui->cprState->setChecked(true);
}

void MainWindow::performCPR(){
    qDebug("In performCPR function!");
}

void MainWindow::shock(){
    qDebug("In shock function!");
}




