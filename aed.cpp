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
        updateTextbox("AED did not power on");
        return false;
    } else if (battery < 20) {
        updateTextbox("AED Audio: Low Battery");
        if(ui->electrodes->currentText() == "T"){
            return true;
        } else {
            updateTextbox("AED Audio: Electodes missing or damaged, AED cannot function");
            return false;
        }
        if(ui->electrodes->currentText() == "T"){
            return true;
        } else {
            updateTextbox("AED Audio: Electodes missing or damaged, AED cannot function");
            return false;
        }
    } else {
        return true;
    }
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
    if (isOn == false) {
        if (selfCheck()) {
            updateTextbox("The AED has been powered on!");
            updateTextbox("Check the responsiveness of the patient.");

            ui->powerButton->setDisabled(true);
            ui->responsivenessButton->setEnabled(true);

            ui->okState->setChecked(true);
            isOn = true;
        } else {
            updateTextbox("The AED is not usable");
        }
    } else {
        isOn = false;
    }
}

void AED::shock(int cprQuality) {
    if (battery >= 10) {
        ui->compressionButton->setEnabled(true);
        ui->breathButton->setEnabled(true);
        ui->shockButton->setEnabled(false);
        updateTextbox("AED Audio: Shock delivered, continue CPR");
        battery = battery - 10;

        int doShock = (QRandomGenerator::global()->generate() % 10); //0 to 9

        qDebug() << "quality needs to pass" << doShock << "to fix heart rate";

        if(cprQuality > doShock){
            patient->setCondition(4);
            updateECG();
        }

        numShocks++;
        ui->batteryLabel->setText("Battery %: " + QString::number(battery));
        ui->numShocksLabel->setText("Number of shocks: " + QString::number(numShocks));

    } else {
        updateTextbox("AED Audio: Battery low, shock not delivered");
        ui->compressionButton->setEnabled(true);
        ui->breathButton->setEnabled(true);
        ui->shockButton->setEnabled(false);
    }
}

