#include "aed.h"

AED::AED()
{



}

AED::AED(Ui::MainWindow*& main, Patient* p, int b, bool a) {
    battery = b;
    isOn = false;
    padsAttached = a;
    padsApplied = false;
    ui = main;
    numShocks = 0;
    patient = p;
}
AED::~AED() {

}

void AED::updateECG(){
    int cond = patient->getCondition();
     if (cond == 0) {
         ui->ecgLabel->setPixmap(QPixmap(":/resources/img/VF.png"));
     } else if (cond == 1) {
         ui->ecgLabel->setPixmap(QPixmap(":/resources/img/VT.png"));
     } else if (cond == 2) {
         ui->ecgLabel->setPixmap(QPixmap(":/resources/img/PEA.png"));
     } else if (cond == 3) {
         ui->ecgLabel->setPixmap(QPixmap(":/resources/img/Asystole.png"));
     } else {
         ui->ecgLabel->setPixmap(QPixmap(":/resources/img/normal.png"));
     }
}


void AED::updateTextbox(QString message){
    ui->aedText->append(message);
}

bool AED::selfCheck() {
    if (battery == 0) {
        updateTextbox("AED: AED did not power on");
        isOn=false;
        return false;
    }
    if(ui->electrodes->currentText() == "F"){
        updateTextbox("AED: Electodes missing or damaged, AED cannot function");
        isOn=false;
        return false;
    }

    if (battery < 20) {
        updateTextbox("AED: Low Battery");
        return true;
    } else {
        return true;
    }
}

bool AED::getState(){
    return isOn;
}

int AED::getBattery() {
    return battery;
}

void AED::setBattery(int b) {
    battery = b;
}

bool AED::getPadsApplied() {
    return padsApplied;
}

void AED::setPadsApplied(bool p) {
    padsApplied = p;
}

int AED::getNumShocks() {
    return numShocks;
}

void AED::resetShocks() {
    numShocks = 0;

}

void AED::powerButton() {
    if (isOn == false) { //Turn the AED on if it's off, either resuming from a previous state or starting for the first time
        if (selfCheck()) {
            ui->aedText->clear();
            updateTextbox("AED: The AED has been powered on!");
            ui->userActionsFrame->setEnabled(true);
            ui->aedDisplayFrame->setEnabled(true);
            ui->powerButton->setDisabled(true);
            ui->powerOff->setEnabled(true);
            isOn = true;
        } else {
            updateTextbox("AED: The AED is not usable");
        }
    } else { //AED was on and is now powered off
        isOn = false;
        ui->aedText->clear();
        ui->ecgLabel->setPixmap(QPixmap(""));
        updateTextbox("AED: The AED has been powered off!");
        ui->powerOff->setEnabled(false);
        ui->powerButton->setEnabled(true);

        ui->aedDisplayFrame->setEnabled(false);
        ui->userActionsFrame->setEnabled(false);
    }
}

void AED::shock(int cprQuality) {
    if (battery >= 10) {
        ui->compressionButton->setEnabled(true);
        ui->breathButton->setEnabled(true);
        ui->shockButton->setEnabled(false);
        battery = battery - 10;

        int doShock = (QRandomGenerator::global()->generate() % 10); //0 to 9

        qDebug() << "quality needs to pass" << doShock << "to fix heart rate";

        if(cprQuality > doShock){ //It's possible that the shock doesn't just set the patient's rhythm to normal (Either to VF, VT Or even PEA)
                updateTextbox("AED: Shock delivered, Sinus Rhytm detected STOP CPR and continue waiting for EMS to arrive");
                patient->setCondition(4);
                ui->compressionButton->setEnabled(false);
                ui->breathButton->setEnabled(false);
                updateECG();
        }
        else{ //Shock didn't set patient to a normal rhythm
            int abnormalRhythm = (QRandomGenerator::global()->generate() % 100);
            if(abnormalRhythm >= 0 && abnormalRhythm <48){ // 47/99 Chance to be VT
                updateTextbox("AED: Shock delivered, Ventricular fibrillation detected continue CPR");
                patient->setCondition(0);
                updateECG();
            }
            else if(abnormalRhythm >=48 && abnormalRhythm <97){ // 48/99 Chance to be VF
                updateTextbox("AED: Shock delivered, Ventricular Tachycardia detected continue CPR");
                patient->setCondition(1);
                updateECG();
            }
            else{ // 4/99 Chance to be PEA (Might change to be higher, but it's boring and doesn't allow shocks)
                updateTextbox("AED: Shock delivered, Pulseless Electrical Activity detected continue CPR");
                patient->setCondition(2);
                updateECG();
            }
        }
        if(battery==0){
            updateTextbox("AED: Battery has been depleted, continue CPR if abnormal rhythm was detected until EMS arrives!");
            ui->powerOff->setEnabled(false);
            ui->disconnectElectrodes->setEnabled(false);
            ui->aedDisplayFrame->setEnabled(false);
            ui->ecgLabel->setPixmap(QPixmap(""));
            ui->userActionsFrame->setEnabled(false);
            isOn=false;
        }

        numShocks++;
        ui->batteryLabel->setText("Battery %: " + QString::number(battery));
        ui->numShocksLabel->setText("Number of shocks: " + QString::number(numShocks));

        }
    else {
        updateTextbox("AED: Battery low, shock not delivered");
        ui->compressionButton->setEnabled(true);
        ui->breathButton->setEnabled(true);
        ui->shockButton->setEnabled(false);
    }
    ui->cprState->setChecked(true);
    ui->noTouchState->setChecked(false);
}

int AED::assessPatient(){ //Returns the condition of the patient (The one matching the displayed ECG)
    updateTextbox("AED: Analysing Patient...");
    updateECG();
    if(patient->getCondition()==0){
        updateTextbox("AED: Patient's condition matches Ventricular fibrillation");
        return 0;
    }
    else if(patient->getCondition()==1){
        updateTextbox("AED: Patient's condition matches Ventricular Tachycardia");
        return 1;
    }
    else if(patient->getCondition()==2){
        updateTextbox("AED: Patient's condition matches Pulseless Electrical Activity");
        return 2;
    }
    else if(patient->getCondition()==3){
        updateTextbox("AED: Patient's condition matches an Asystole");
        return 3;
    }
    else{
        updateTextbox("AED: Patient's condition matches a Normal Sinus Rhythm");
        return 4;
    }
}

void AED::displayBattery(){
    ui->batteryLabel->setText("Remaining battery: " + QString::number(getBattery()));
}

