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
    bool selfCheck(); //Function to check AED's battery and status of defib pads before functioning like normal

    void powerButton();

    void updateTextbox(QString);
    void updateECG();
    int assessPatient(); //Returns the patient's condition
    void displayBattery();

    void shock(int);

    int getBattery();
    void setBattery(int);

    bool getPadsApplied(); //Returns whether or not both pads are applied to the patient
    void setPadsApplied(bool);

    int getNumShocks();
    bool getState(); //Returns whether or not the AED is on
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
