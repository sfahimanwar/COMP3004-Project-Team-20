#ifndef PATIENT_H
#define PATIENT_H

#include <QDebug>

class Patient
{
public:
    Patient(int, int, bool, bool, bool, bool, int);
    void receiveCPR();
    void receiveShock();
    enum condition {
      ventricularFibrillation,
      ventricularTachycardia,
      nonShockable,
      none
    };

    enum bodyType {
        normal,
        child
    };

    bool getIsEven();
    bool getSafeRange();
    bool getResponsive();
    bool getBreathing();

private:
    int patientCondition;
    //bodyType body;
    int body;
    int pulse;
    bool isEven;
    bool isSafeRange;
    bool isResponsive;
    bool isBreathing;
};

#endif // PATIENT_H
