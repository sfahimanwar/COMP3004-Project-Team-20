#ifndef AED_H
#define AED_H

#include <patient.h>
#include "ui_mainwindow.h"
#include <QRandomGenerator>

class AED
{
public:
    AED();
    AED(Ui::MainWindow*&, Patient*, int, bool);
    ~AED();
    bool selfCheck();

    void powerButton();

    void updateTextbox(QString);
    void updateECG();
    int assessPatient();
    void displayBattery();

    void shock(int);

    int getBattery();
    void setBattery(int);

    bool getPadsApplied();
    void setPadsApplied(bool);

    int getNumShocks();
    bool getState();
    void resetShocks();



private:
    int battery;
    bool isOn;
    bool padsAttached;
    bool padsApplied;
    int numShocks;
    Ui::MainWindow * ui;
    Patient* patient;

};

#endif // AED_H
