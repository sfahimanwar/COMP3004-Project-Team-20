#include "patient.h"

Patient::Patient(int b, int p, bool even, bool safeRange, bool response, bool breathing){
    patientCondition = ventricularFibrillation; // TODO: Determine condition
    body = b;
    pulse = p;
    isEven = even;
    isSafeRange = safeRange;
    isResponsive = response;
    isBreathing = breathing;
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
