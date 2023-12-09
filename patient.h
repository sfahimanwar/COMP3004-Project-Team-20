#ifndef PATIENT_H
#define PATIENT_H

#include <QDebug>

class Patient
{
public:
    Patient(int, int, bool, bool, bool, bool, bool, int);
    void receiveCPR();
    void receiveShock();

    bool getIsEven();
    bool getSafeRange();
    bool getResponsive();
    bool getBreathing();
    int getCondition();
    bool getQRS();
    int getPStrength();
    bool getChecked();

    void setCondition(int);
    void setChecked(bool);

private:
    int patientCondition;
    int body;
    int pulse;
    bool isEven;
    bool isSafeRange;
    bool isResponsive;
    bool isBreathing;
    bool QRS;
    int pulseStrength;
    bool isChecked;
};

#endif // PATIENT_H
