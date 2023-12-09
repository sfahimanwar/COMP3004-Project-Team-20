#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QLayout>
#include <QtMath>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , emsArrived(false), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->aedDisplay->setPixmap(QPixmap(":/resources/img/aed.jpg"));

    ui->pulseSetBox->setMaximum(MAX_HEART_RATE); // Apparently the highest heart rate ever recorded (A good upper bound).

    //EMS Setup
    emsTimer.setSingleShot(true); // Timer will only fire once as EMS only needs to arrive just once
    ui->emsResetButton->hide();

    //Initial Patient Button Setup
    connect(ui->pulseSetBox, SIGNAL(valueChanged(int)), this, SLOT(updateFromOther())); //All of these update the buttons available after changing one of their values
    connect(ui->pulseEvenBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFromOther())); //This is because some states are dependant on others (Can't have VT if heart rate is 0)
    connect(ui->pulseStrengthBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFromOther()));
    connect(ui->hasQRSComplexBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFromOther()));
    connect(ui->isBreathingBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFromOther()));
    updateFromOther();

    this->aed = new AED();

    // Connects all user action buttons
    connect(ui->powerButton, SIGNAL(released()), this, SLOT(powerOn()));
    connect(ui->responsivenessButton, SIGNAL(released()), this, SLOT(checkResponse()));
    connect(ui->helpButton, SIGNAL(released()), this, SLOT(callEMS()));
    connect(ui->openAirwaysButton, SIGNAL(released()), this, SLOT(openAirways()));
    connect(ui->checkBreathingButton, SIGNAL(released()), this, SLOT(checkBreathing()));
    connect(ui->openChestButton, SIGNAL(released()), this, SLOT(clearChest()));
    connect(ui->attachDefibButton, SIGNAL(released()), this, SLOT(applyPads()));
    connect(ui->moveBackButton, SIGNAL(released()), this, SLOT(moveAway()));
    connect(ui->shockButton, SIGNAL(released()), this, SLOT(shock()));
    connect(ui->createPatientButton, SIGNAL(released()), this, SLOT(beginSimulation()));
    connect(ui->powerOff, SIGNAL(released()), this, SLOT(powerOff()));

    // CPR buttons
    connect(ui->compressionButton, SIGNAL(released()), this, SLOT(performCPR()));
    connect(ui->breathButton, SIGNAL(released()), this, SLOT(performCPR()));

    // Reset buttons
    connect(ui->emsResetButton, SIGNAL(released()), this, SLOT(reset()));
    connect(ui->resetButton, SIGNAL(released()), this, SLOT(reset()));

    // Disables all user action buttons
    ui->powerButton->setDisabled(true);
    ui->responsivenessButton->setDisabled(true);
    ui->helpButton->setDisabled(true);
    ui->openAirwaysButton->setDisabled(true);
    ui->checkBreathingButton->setDisabled(true);
    ui->openChestButton->setDisabled(true);
    ui->attachDefibButton->setDisabled(true);
    ui->moveBackButton->setDisabled(true);
    ui->shockButton->setDisabled(true);
    ui->safetyScenarioFrame->setDisabled(true);


    //EMS Related Connections
    connect(&emsTimer, SIGNAL(timeout()), this, SLOT(emsArrives()));
    //Need to add EMS reset button functionality, will combine with base reset functionality when complete.

    ui->userActionsFrame->setDisabled(true);
    ui->aedAudioFrame->setDisabled(true);
    ui->aedDisplayFrame->setDisabled(true);
    ui->cprFrame->setDisabled(true);
    ui->patientFrame->setDisabled(true);
    ui->ecgFrame->setDisabled(true);
    ui->configFrame->setEnabled(true);
    ui->infoFrame->setDisabled(true);

    QString compressions('c');
    QString breaths('B');
    compressions = compressions.repeated(NUM_COMPRESSIONS);
    breaths = breaths.repeated(NUM_BREATHS);
    idealPattern = compressions + breaths + compressions + breaths;
    qDebug() << "idealPattern:" << idealPattern;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reset(){
    ui->aedText->clear();
    showAll();

    // Hide all "EMS arrived" elements
    ui->emsResetButton->hide();
    ui->emsArrivedLabel->hide();
    ui->emsArrivedText->hide();

    ui->patientLabel->setPixmap(QPixmap()); // Reset patient image
    ui->ecgLabel->setPixmap(QPixmap());     // Reset ECG reading

    //Enable CPR buttons (They won't show at first because it's frame is disabled)
    ui->compressionButton->setEnabled(true);
    ui->breathButton->setEnabled(true);

    // Disable all frames but the config
    ui->userActionsFrame->setDisabled(true);
    ui->aedAudioFrame->setDisabled(true);
    ui->aedDisplayFrame->setDisabled(true);
    ui->cprFrame->setDisabled(true);
    ui->patientFrame->setDisabled(true);
    ui->ecgFrame->setDisabled(true);
    ui->configFrame->setEnabled(true);
    ui->infoFrame->setDisabled(true);
    ui->safetyScenarioFrame->setDisabled(true);

    ui->emsTimeLabel->setText("Time until EMS arrives: --- seconds");
    ui->batteryLabel->setText("Remaining battery: ---");
    ui->numShocksLabel->setText("Number of shocks: -");


    // Disable all user action buttons
    QObject* uiElement;
    foreach(uiElement, ui->userActionsFrame->children()){
        QWidget* uiWidget = qobject_cast<QWidget*>(uiElement);
        if(uiWidget != nullptr){
            uiWidget->setDisabled(true);
        }
    }

    // Disable all radio buttons
    foreach(uiElement, ui->aedDisplayFrame->children()){
        QRadioButton* aedLights = qobject_cast<QRadioButton*>(uiElement);
        if(aedLights != nullptr){
            aedLights->setChecked(false);
        }
    }
    // These buttons sit on their own so I elected to just uncheck/disable them manually instead of using a foreach loop like the above
    ui->leftElectrode->setChecked(false);
    ui->leftElectrode->setDisabled(true);
    ui->rightElectrode->setChecked(false);
    ui->rightElectrode->setDisabled(true);

    //Patient No longer checked for responsiveness
    patient->setChecked(false);

    emsTimer.stop();    //TODO: KNOWN ISSUE: If the timer is going, and the user hits "reset," everything will reset correctly but after a few seconds the ems timer will update anyway and display 0.
}



void MainWindow::beginSimulation(){
    // Get values from buttons to create patient and begin simulation
    int body = 0;
    int pulse = ui->pulseSetBox->value();
    bool pulseSafeRange = false;
    bool regPulse = false;
    bool response = false;
    bool breathing = false;
    bool q = false;
    int pulseStrength=0;

    ui->powerButton->setEnabled(true);
    ui->patientFrame->setEnabled(true);
    ui->aedDisplayFrame->setEnabled(false);
    ui->infoFrame->setEnabled(true);
    ui->responsivenessButton->setEnabled(true);
    ui->okState->setChecked(true);

    if (ui->bodyBox->currentText() == "Child"){
        body = 1;
        ui->patientLabel->setPixmap(QPixmap(":/resources/img/child.jpg"));
    }else{
        ui->patientLabel->setPixmap(QPixmap(":/resources/img/dummy.jpg"));
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

    if(ui->hasQRSComplexBox->currentText() == "T"){
           q = true;
       }

    if(ui->pulseStrengthBox->currentText() == "Weak"){
        pulseStrength = 1;
    }

    patient= new Patient(body, pulse, regPulse, pulseSafeRange, response, breathing, q, pulseStrength);
    if (ui->electrodes->currentText() == "T") {
         aed = new AED(ui, patient, ui->batteryConfig->value(), true);
    } else {
        aed = new AED(ui, patient, ui->batteryConfig->value(), false);
    }

    ui->userActionsFrame->setEnabled(false);
    ui->safetyScenarioFrame->setEnabled(true);
    ui->aedAudioFrame->setEnabled(true);

    qDebug() << "Created a patient with values body:" << body << "pulse:" << pulse << " pulseSafeRange" << pulseSafeRange << "regPulse:" << regPulse << "response:" << response << "breathing:" << breathing;
    qDebug() << "Patient condition is" << patient->getCondition();
    ui->configFrame->setDisabled(true);


}

void MainWindow::powerOn(){
    aed->powerButton();
    aed->displayBattery();
    if(patient->getChecked()==false){
       aed->updateTextbox("AED: Check the responsiveness of the patient.");
    }
    else{
       aed->updateTextbox("AED: Previous Session Restored, please continue from the section displayed on the device");
    }
}

void MainWindow::powerOff(){
    aed->powerButton();
}

void MainWindow::checkResponse(){
    patient->setChecked(true);
    bool response = patient->getResponsive();
    if (response){
        aed->updateTextbox("AED: The patient is responsive--do NOT use the AED! Wait for help to arrive!");
    }else{
        aed->updateTextbox("AED: The patient is unresponsive. Call for help and prepare to administer CPR.");
    }

        ui->responsivenessButton->setDisabled(true);
        ui->helpButton->setEnabled(true);

        ui->okState->setChecked(false);
        ui->emergencyState->setChecked(true);
}

void MainWindow::callEMS(){
    int numMinutes = (QRandomGenerator::global()->generate() % 4)+ 2; // 2-5 minute wait time for EMS
    emsTimer.start(numMinutes*60000);
    ui->emsTimeLabel->setText("Time until EMS arrives: " + QString::number(emsTimer.remainingTime()/1000) + " seconds");
    qDebug() << "EMSTimer has been set to" << numMinutes << "Minutes";

    //Setting up minute counter (Displays in console how long until EMS arrives (10 second intervals)
    connect(&minuteCounter, &QTimer::timeout, [this]() { //Function is small so just put it inside of a lambda function
        ui->emsTimeLabel->setText("Time until EMS arrives: " + QString::number(emsTimer.remainingTime()/1000) + " seconds");
        if(emsTimer.remainingTime()<=0){
            minuteCounter.stop();
        }
    });
  
    minuteCounter.start(1000); //Displaying every second
    ui->helpButton->setDisabled(true);

    if(!patient->getResponsive()){
        aed->updateTextbox("AED: EMS has been called, and will be arriving shortly! Open the patient's airways.");

        ui->openAirwaysButton->setEnabled(true);

        ui->emergencyState->setChecked(false);
        ui->airwayState->setChecked(true);
    }
    else{
        aed->updateTextbox("AED: EMS has been called, and will be arriving shortly!");
    }
}

void MainWindow::openAirways(){
    aed->updateTextbox("AED: The patient's airways have been opened, check if the patient is breathing.");

    ui->openAirwaysButton->setDisabled(true);
    ui->checkBreathingButton->setEnabled(true);

    ui->airwayState->setChecked(false);
    ui->breathState->setChecked(true);

}

void MainWindow::checkBreathing(){
    if(patient->getBreathing() == false){
        aed->updateTextbox("AED: The patient is not breathing. Prepare to apply defibrilator pads. First, clear the patient's chest.");
    }else{
        aed->updateTextbox("AED: The patient is breathing. TODO: SOMETHING HERE");
    }
    ui->checkBreathingButton->setDisabled(true);
    ui->openChestButton->setEnabled(true);

    ui->breathState->setChecked(false);
    ui->padState->setChecked(true);

}

void MainWindow::clearChest(){
    aed->updateTextbox("AED: Patient's chest has been cleared. Attach the patient's defibrillator pads now.");

    ui->rightElectrode->setEnabled(true);
    ui->leftElectrode->setEnabled(true);

    ui->openChestButton->setDisabled(true);
    ui->attachDefibButton->setEnabled(true);

    ui->breathState->setChecked(false);
    ui->padState->setChecked(true);

}

void MainWindow::applyPads(){
    bool electrode1 = ui->leftElectrode->isChecked();
    bool electrode2 = ui->rightElectrode->isChecked();
    if (electrode1 && electrode2){
        aed->setPadsApplied(true);
        aed->updateTextbox("AED: Both defibrillator pads are in now place. Move back, do not touch the patient!");
        ui->rightElectrode->setDisabled(true);
        ui->leftElectrode->setDisabled(true);

        ui->attachDefibButton->setDisabled(true);

        if(aed->assessPatient()==4){
            aed->updateTextbox("AED: DO NOT start CPR, continue waiting for EMS to arrive");
        }
        else{
            ui->moveBackButton->setEnabled(true);

            ui->padState->setChecked(false);
            ui->noTouchState->setChecked(true);
        }

    }else{
        aed->setPadsApplied(false);
        aed->updateTextbox("AED: Defibrillator pads not attached!");
    }
}

void MainWindow::moveAway(){
    aed->updateTextbox("AED: Prepare to administer CPR! Compress the patient's chest " + QString::number(NUM_COMPRESSIONS) + " times, followed by " + QString::number(NUM_BREATHS) + " breaths." + " Repeat the pattern twice, and then wait for AED assessment.");
    aed->updateTextbox("AED: Also make sure to reach a full depth of 2.4 inches when compressing");
    ui->moveBackButton->setDisabled(true);
    ui->shockButton->setEnabled(true);
    ui->cprFrame->setEnabled(true);

    ui->noTouchState->setChecked(false);
    ui->cprState->setChecked(true);
    ui->shockButton->setEnabled(false);
}


void MainWindow::performCPR(){
    int idealLength = 2*NUM_COMPRESSIONS + 2*NUM_BREATHS;
    int cprCycleLength = cprString.length();
    cprQuality = 0;

    /* As the user performs CPR (presses the compression/breath) buttons, signals are sent to this function which are then used to build a QString called
     * cprString. When cprString's length is equal to idealLength, the CPR cycle is considered "finished," the patient can be analyzed, and feedback on
     * CPR is provided.
     *
     * CPR is rated as good/ok/poor as an integer from 0 to 3 depending on if the CPR meets the following conditions:
     *  - 2 breaths are administered in the middle and 2 at the end
     *  - There is a reasonable number of compressions administered (NUMCOMPRESSIONS ±1)
     *  - There is a reasonable number of breaths administered (NUMBREATHS ±1)
     *
     * Examples:
     *      ccccBBccccBB = quality 3/GOOD (Ideal)
     *      cBccBBccccBB = quality 3/GOOD (extra breath, but within acceptable wiggle room to still be considered good CPR)
     *      ccccBBcBcccB = quality 2/OKAY (correct number of breaths, missing breaths at the end)
     *      ccccBBcBBcBB = quality 1/POOR (two extra breaths, not within acceptable wiggle room to be considered ok CPR)
     *      ccBBcBcBBBcc = quality 0/POOR (Missing middle and end breaths, too many breaths, too few compressions)
     *
     *This CPR rating is then multiplied by a number between 0 and 1 and floored (This number comes from the depth of compression slider)
     *
     * If you want to "increase the realism" you can change NUMCOMPRESSIONS in mainwindow.h to 30 which is more accurate to real life, but a huge pain
     * since you gotta press the buttons 64 times per cycle. However, this function is designed to handle those changes, so if you love pressing buttons, you can.
     */


    if(QObject::sender()->objectName() == "compressionButton"){
        cprString.append("c");
    }else{
        cprString.append("B");
    }

    // Cycle considered "finished," AED will analyze patient and provide feedback
    if(cprCycleLength + 1 == idealLength){
        qDebug() << "Checking cpr quality: your string is" << cprString << "and the ideal string is" << idealPattern;
        if(cprString[NUM_COMPRESSIONS] == 'B' && cprString[NUM_COMPRESSIONS + 1] == 'B' && cprString[2*NUM_COMPRESSIONS+2] == 'B' && cprString[2*NUM_COMPRESSIONS +3] == 'B'){    //breaths in correct spots
            qDebug() << "gained 1 quality for proper breath ordering";
            cprQuality++;
        }

        QChar c;
        int numCompressions = 0;
        int numBreaths = 0;
        foreach(c, cprString){
            if(c == 'c'){
                numCompressions++;
            }if(c == 'B'){
                numBreaths++;
            }
        }

        if(2*NUM_COMPRESSIONS-1 <= numCompressions && numCompressions <= 2*NUM_COMPRESSIONS+1){
            cprQuality++;
            qDebug() << "gained 1 quality for acceptable compressions amount";
        }

        if(2*NUM_BREATHS-1 <= numBreaths && numBreaths <= 2*NUM_BREATHS+1){
            cprQuality++;
            qDebug() << "gained 1 quality for acceptable breaths amount";
        }

        cprQuality = qFloor((float)cprQuality * ((float)(ui->compressionSlider->value()+1))/(float)100); // Factoring in the slider

        qDebug() << "This round of CPR has a quality rating of" << cprQuality;
        cprString = "";


        aed->updateTextbox("AED: Your CPR performance was rated " + QString::number(cprQuality) + "/3");
        if(cprQuality == 3){
            aed->updateTextbox("AED: Keep up the good work!");
        }else{
            aed->updateTextbox("AED: Recall, for good CPR: Compress the patient's chest " + QString::number(NUM_COMPRESSIONS) + " times, followed by " + QString::number(NUM_BREATHS) + " breaths." + " Repeat the pattern twice, and then wait for AED assessment.");
            aed->updateTextbox("AED: Also make sure to reach a full depth of 2.4 inches when compressing");
        }
        int condition = aed->assessPatient();

        // Assessment of CPR cycle completed, now determine if a shock is necessary:
        if ((condition == 0) || (condition == 1)) {
            aed->updateTextbox("AED: Shock advised");
            ui->compressionButton->setEnabled(false);
            ui->breathButton->setEnabled(false);
            ui->shockButton->setEnabled(true);
        }
        else {
            aed->updateTextbox("AED: Shock not advised, continue CPR");
        }
    }
}


void MainWindow::shock(){
   aed->shock(cprQuality);
}

void MainWindow::emsArrives(){
    //Hiding all elements
    hideAll();

    //Showing EMS elements
    ui->emsArrivedLabel->setPixmap(QPixmap(":/resources/img/ems.jpg"));
    ui->emsArrivedLabel->show();
    ui->emsArrivedText->show();
    ui->emsArrivedText->setText("EMS has arrived, Good Job!");
    ui->emsResetButton->show();

    emsArrived = true; // EMS Has arrived (Check during CPR functions)

    qDebug() << "EMS Has Arrived, they will now take over CPR"; // Printing in console because AED window is only for it's audio.
}

void MainWindow::hideAll(){ // Helper function to just hide all elements that are children of the central widget (Essentially everything)
    QObject* uiElement;
    foreach(uiElement, ui->centralwidget->children()){
        QWidget* uiWidget = qobject_cast<QWidget*>(uiElement);
        if(uiWidget != nullptr){
            uiWidget->hide();
        }
    }
}

void MainWindow::showAll(){  // Helper function to show all elements
    QObject* uiElement;
    foreach(uiElement, ui->centralwidget->children()){
        QWidget* uiWidget = qobject_cast<QWidget*>(uiElement);
        if(uiWidget != nullptr){
            uiWidget->show();
        }
    }
}

void MainWindow::setHighButtons(){
    ui->pulseEvenBox->setEnabled(true);
    ui->responsivenessBox->setEnabled(true);
    ui->isBreathingBox->setEnabled(true);
    ui->hasQRSComplexBox->setEnabled(true);
    ui->pulseStrengthBox->setEnabled(true);

    if(ui->pulseStrengthBox->currentText() == "Weak"){ //Pulse Strength is weak, automatically PEA, disable other elements
        ui->pulseEvenBox->setCurrentIndex(0);
        ui->pulseEvenBox->setDisabled(true);
        ui->responsivenessBox->setCurrentIndex(1); //If pulse strength is weak this implies the patient is unconscious
        ui->responsivenessBox->setDisabled(true);
        ui->isBreathingBox->setCurrentIndex(1); //If pulse strength is weak this implies the patient is not breathing
        ui->isBreathingBox->setDisabled(true);
        ui->hasQRSComplexBox->setCurrentIndex(0);
        ui->hasQRSComplexBox->setDisabled(true);
        return;
    }
    else{
        ui->pulseEvenBox->setEnabled(true);
        ui->responsivenessBox->setEnabled(true);
        ui->isBreathingBox->setEnabled(true);
        ui->hasQRSComplexBox->setEnabled(true);
        ui->pulseStrengthBox->setEnabled(true);
    }

    if(ui->pulseEvenBox->currentText() == "F"){ //Pulse is not even, automatically VF (Rest of the values implied)
        ui->responsivenessBox->setCurrentIndex(1);
        ui->responsivenessBox->setDisabled(true);
        ui->isBreathingBox->setCurrentIndex(1);
        ui->isBreathingBox->setDisabled(true);
        ui->hasQRSComplexBox->setCurrentIndex(1);
        ui->hasQRSComplexBox->setDisabled(true);
        return;
    }
    else{
        ui->pulseEvenBox->setEnabled(true);
        ui->responsivenessBox->setEnabled(true);
        ui->isBreathingBox->setEnabled(true);
        ui->hasQRSComplexBox->setEnabled(true);
        ui->pulseStrengthBox->setEnabled(true);
    }

    if(ui->hasQRSComplexBox->currentText() == "F"){ //No QRS complex, automatically VT (Rest of values implied)
        ui->responsivenessBox->setCurrentIndex(1);
        ui->responsivenessBox->setDisabled(true);
        ui->isBreathingBox->setCurrentIndex(1);
        ui->isBreathingBox->setDisabled(true);
        return;
    }
    else{
        ui->pulseEvenBox->setEnabled(true);
        ui->responsivenessBox->setEnabled(true);
        ui->isBreathingBox->setEnabled(true);
        ui->hasQRSComplexBox->setEnabled(true);
        ui->pulseStrengthBox->setEnabled(true);
    }

    if(ui->isBreathingBox->currentText() == "F"){
        ui->responsivenessBox->setCurrentIndex(1); //If patient is not breathing this implies they are unconscious
        ui->responsivenessBox->setDisabled(true);
        return;
    }
    else{
        ui->pulseEvenBox->setEnabled(true);
        ui->responsivenessBox->setEnabled(true);
        ui->isBreathingBox->setEnabled(true);
        ui->hasQRSComplexBox->setEnabled(true);
        ui->pulseStrengthBox->setEnabled(true);
    }

}

void MainWindow::setNormalButtons(){
    ui->pulseEvenBox->setCurrentIndex(0); //Can't have a non-even pulse (VF) with a normal heart rate
    ui->pulseEvenBox->setDisabled(true);
    ui->responsivenessBox->setEnabled(true);
    ui->isBreathingBox->setEnabled(true);
    ui->hasQRSComplexBox->setCurrentIndex(0); //Can't not have a QRS complex (VT) with a normal heart rate
    ui->hasQRSComplexBox->setDisabled(true);
    ui->pulseStrengthBox->setEnabled(true);

    if(ui->pulseStrengthBox->currentText() == "Weak"){ //Pulse Strength is weak, automatically PEA, disable other elements
        ui->responsivenessBox->setCurrentIndex(1);
        ui->responsivenessBox->setDisabled(true); //If pulse strength is weak this implies the patient is unconscious
        ui->isBreathingBox->setCurrentIndex(1);
        ui->isBreathingBox->setDisabled(true); //If pulse strength is weak this implies the patient is not breathing
        return;
    }
    else{
        ui->responsivenessBox->setEnabled(true);
        ui->isBreathingBox->setEnabled(true);
    }

    if(ui->isBreathingBox->currentText() == "F"){
        ui->responsivenessBox->setCurrentIndex(1); //If patient is not breathing this implies they are unconscious
        ui->responsivenessBox->setDisabled(true);
        return;
    }
    else{
        ui->responsivenessBox->setEnabled(true);
    }



}

void MainWindow::updateFromOther(){ //Initial button availability is based on heart rate of patient

    if(ui->pulseSetBox->value() == 0){ //If Heart Rate is flatlined
        ui->pulseEvenBox->setCurrentIndex(0);
        ui->pulseEvenBox->setDisabled(true);
        ui->responsivenessBox->setCurrentIndex(1);
        ui->responsivenessBox->setDisabled(true);
        ui->isBreathingBox->setCurrentIndex(1);
        ui->isBreathingBox->setDisabled(true);
        ui->hasQRSComplexBox->setCurrentIndex(1);
        ui->hasQRSComplexBox->setDisabled(true);
        ui->pulseStrengthBox->setCurrentIndex(1);
        ui->pulseStrengthBox->setDisabled(true);
    }
    else if(ui->pulseSetBox->value()>=120){ //If Heart Rate is 120 or over (Now possible for VF and VT)
        setHighButtons();
    }
    else{ //Heart rate between 0 and 120 (VF and VT not possible (But Normal and PEA still possible))
        setNormalButtons();
    }
}



