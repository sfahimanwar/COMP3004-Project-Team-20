#include "patient.h"

Patient::Patient(int b, int p, bool even, bool safeRange, bool response, bool breathing, bool q){
    body = b;
    pulse = p;
    isEven = even;
    isSafeRange = safeRange;
    isResponsive = response;
    isBreathing = breathing;
    QRS = q;

    //0 = VF
    //1 = VT
    //2 = PEA
    //3 = Asystole
    //4 = Normal

        // asystole
        if (pulse == 0) {
            patientCondition = 3;
            return;
        }

        if ((isEven == true) && (pulse>170) && (QRS == false)) {
                patientCondition = 1;
                return;
        }
        if ((isEven == true) && (QRS == true)) {
                patientCondition = 2;
                return;
        }
        if (isEven == false) {
            patientCondition = 0;
            return;
        }
        patientCondition = 4;
        return;


        //even pulse & under 120 = VT/PEA
        //PEA has QRS (VT does not)

        //VF = not even

        //Else = normal
}

void Patient::receiveCPR(){
    qDebug() << "In receiveCPR";
}

void Patient::receiveShock(){
    qDebug() << "In receiveShock";
}

bool Patient::getIsEven(){
    return isEven;
}

bool Patient::getSafeRange(){
    return isSafeRange;
}

bool Patient::getResponsive(){
    return isResponsive;
}

bool Patient::getBreathing(){
    return isBreathing;
}

int Patient::getCondition() {
    return patientCondition;
}
