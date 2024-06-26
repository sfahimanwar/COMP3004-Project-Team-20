#include "patient.h"

Patient::Patient(int b, int p, bool even, bool safeRange, bool response, bool breathing, bool qrs, int pStrength){
    body = b;
    pulse = p;
    isEven = even;
    isSafeRange = safeRange;
    isResponsive = response;
    isBreathing = breathing;
    QRS = qrs;
    pulseStrength = pStrength;
    isChecked=false;

    //0 = VF
    //1 = VT
    //2 = PEA
    //3 = Asystole
    //4 = Normal

        // asystole (Pulse is <=30, effectively no heart rate)
        if (pulse <=30) {
            patientCondition = 3;
            return;
        }

        //PEA (Heart pulse strength is very weak)
        if(pulseStrength == 1){
            patientCondition=2;
            return;
        }

        //VT (Tachycardia when the pulse is evenly spaced but NO QRS complex found (Must have a high heart rate as well but this is enforced within mainwindow)
        if ((isEven == true) && (QRS == false)) {
                patientCondition = 1;
                return;
        }

        //Normal (When a QRS complex is found and the heart rate is normal)
        if ((isEven == true) && (QRS == true)) {
                patientCondition = 4;
                return;
        }

        //VF (Fibrillation when the pulse is NOT evenly spaced (Must have a high heart rate as well but this is enforced within mainwindow)
        if (isEven == false) {
            patientCondition = 0;
            return;
        }

}

void Patient::setChecked(bool state){
    isChecked = state;
}

bool Patient::getChecked(){
    return isChecked;
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

bool Patient::getQRS(){return QRS;}

int Patient::getPStrength(){return pulseStrength;}

void Patient::setCondition(int c){
    patientCondition = c;
}
