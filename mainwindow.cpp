#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , emsArrived(false), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->aedDisplay->setPixmap(QPixmap(":/resources/img/aed.jpg"));

    ui->pulseSetBox->setMaximum(600); // Apparently the highest heart rate ever recorded (A good upper bound).

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


    //EMS Related Connections
    connect(&emsTimer, SIGNAL(timeout()), this, SLOT(emsArrives()));
    //Need to add EMS reset button functionality, will combine with base reset functionality when complete.

    updateTextbox("To begin using the AED, press the power button.");

    ui->userActionsFrame->setDisabled(true);
    ui->aedAudioFrame->setDisabled(true);
    ui->aedDisplayFrame->setDisabled(true);
    ui->cprFrame->setDisabled(true);
    ui->patientFrame->setDisabled(true);
    ui->ecgFrame->setDisabled(true);
    ui->configFrame->setEnabled(true);

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

    // Disable all frames but the config
    ui->userActionsFrame->setDisabled(true);
    ui->aedAudioFrame->setDisabled(true);
    ui->aedDisplayFrame->setDisabled(true);
    ui->cprFrame->setDisabled(true);
    ui->patientFrame->setDisabled(true);
    ui->ecgFrame->setDisabled(true);
    ui->configFrame->setEnabled(true);

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

    emsTimer.stop();
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
    ui->aedDisplayFrame->setEnabled(true);

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

    ui->userActionsFrame->setEnabled(true);
    ui->aedAudioFrame->setEnabled(true);

    qDebug() << "Created a patient with values body:" << body << "pulse:" << pulse << " pulseSafeRange" << pulseSafeRange << "regPulse:" << regPulse << "response:" << response << "breathing:" << breathing;
    ui->configFrame->setDisabled(true);


}

void MainWindow::updateTextbox(QString message){
    ui->aedText->append(message);
}


bool MainWindow::selfCheck() {
    battery = ui->batteryConfig->value();
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




void MainWindow::powerOn(){
    if (selfCheck()) {


    updateTextbox("The AED has been powered on!");
    updateTextbox("Check the responsiveness of the patient.");

    ui->powerButton->setDisabled(true);
    ui->responsivenessButton->setEnabled(true);

    ui->okState->setChecked(true);
    } else {

    }
}

void MainWindow::checkResponse(){
    bool response = patient->getResponsive();
    if (response){
        updateTextbox("The patient is responsive--do NOT use the AED! Wait for help to arrive!");
        //TODO: stuff here
    }else{
        updateTextbox("The patient is unresponsive. Call for help and prepare to administer CPR.");

        ui->responsivenessButton->setDisabled(true);
        ui->helpButton->setEnabled(true);

        ui->okState->setChecked(false);
        ui->emergencyState->setChecked(true);
    }

}

void MainWindow::callEMS(){


    updateTextbox("EMS has been called, and will be arriving shortly!");
    int numMinutes = (QRandomGenerator::global()->generate() % 4)+ 2; // 2-5 minute wait time for EMS
    emsTimer.start(numMinutes*60000);
    qDebug() << "EMSTimer has been set to" << numMinutes << "Minutes";

    //Setting up minute counter (Displays in console how long until EMS arrives (10 second intervals)
    connect(&minuteCounter, &QTimer::timeout, [this]() { //Function is small so just put it inside of a lambda function
        qDebug() << "Time until EMS arrives: " << (emsTimer.remainingTime()/1000) << "Seconds";
        if(emsTimer.remainingTime()<=0){
            minuteCounter.stop();
        }
    });
    minuteCounter.start(10000);

    updateTextbox("Open the patient's airways.");

    ui->helpButton->setDisabled(true);
    ui->openAirwaysButton->setEnabled(true);

    ui->emergencyState->setChecked(false);
    ui->airwayState->setChecked(true);
}

void MainWindow::openAirways(){
    updateTextbox("The patient's airways have been opened, check if the patient is breathing.");

    ui->openAirwaysButton->setDisabled(true);
    ui->checkBreathingButton->setEnabled(true);

    ui->airwayState->setChecked(false);
    ui->breathState->setChecked(true);

}

void MainWindow::checkBreathing(){
    if(patient->getBreathing() == false){
        updateTextbox("The patient is not breathing. Prepare to apply defibrilator pads. First, clear the patient's chest.");
    }else{
        updateTextbox("The patient is breathing. TODO: SOMETHING HERE");
    }
    ui->checkBreathingButton->setDisabled(true);
    ui->openChestButton->setEnabled(true);

    ui->breathState->setChecked(false);
    ui->padState->setChecked(true);

}

void MainWindow::clearChest(){
    updateTextbox("Patient's chest has been cleared.");
    updateTextbox("Attach the patient's defibrillator pads now.");

    ui->rightElectrode->setEnabled(true);
    ui->leftElectrode->setEnabled(true);

    ui->openChestButton->setDisabled(true);
    ui->attachDefibButton->setEnabled(true);

    ui->breathState->setChecked(false);
    ui->padState->setChecked(true);

}

void MainWindow::updateECG(){
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

void MainWindow::applyPads(){
    bool electrode1 = ui->leftElectrode->isChecked();
    bool electrode2 = ui->rightElectrode->isChecked();
    if (electrode1 && electrode2){
        updateTextbox("Both defibrillator pads are in now place. Move back, do not touch the patient!");
        ui->rightElectrode->setDisabled(true);
        ui->leftElectrode->setDisabled(true);

        ui->attachDefibButton->setDisabled(true);
        ui->moveBackButton->setEnabled(true);

        ui->padState->setChecked(false);
        ui->noTouchState->setChecked(true);

        updateECG();

    }else{
        updateTextbox("Defibrillator pads not attached!");
    }
}

void MainWindow::moveAway(){
    isTouching = false;
    updateTextbox("Prepare to administer CPR [CPR INSTRUCTIONS HERE]");

    ui->moveBackButton->setDisabled(true);
    ui->shockButton->setEnabled(true);
    ui->cprFrame->setEnabled(true);

    ui->noTouchState->setChecked(false);
    ui->cprState->setChecked(true);
    ui->shockButton->setEnabled(false);
}






void MainWindow::performCPR(){
    std::string idealBreaths;
    for(int i = 0; i < NUM_BREATHS; i++) {
        idealBreaths += "1";
    }
    std::string idealCompressions;
    for(int i = 0; i < NUM_COMPRESSIONS; i++) {
        idealCompressions += "0";
    }

    if(QObject::sender()->objectName() == "compressionButton"){
        cprString.append("0");
        compressionsCount = compressionsCount + 1;
    }else{
        cprString.append("1");
        breathsCount = breathsCount + 1;
    }
    std::string idealString =  idealCompressions + idealBreaths + idealCompressions + idealBreaths;
    qDebug() << "CPRSTRING " << QString::fromStdString(cprString) << " IDEAL STRING " << QString::fromStdString(idealString);

    if ((breathsCount + compressionsCount) == (NUM_BREATHS + NUM_COMPRESSIONS)*2) {
        if (cprString == idealString) {
            qDebug() << "GOOD CPR";
        } else {
            qDebug() << "BAD CPR";

        }
        breathsCount = 0;
        compressionsCount = 0;
        cprString = "";
        if ((patient->getCondition() == 0) || (patient->getCondition() == 1)) {
            updateTextbox("AED Audio: Shock advised");
            ui->compressionButton->setEnabled(false);
            ui->breathButton->setEnabled(false);
            ui->shockButton->setEnabled(true);
        } else {
            updateTextbox("AED Audio: Shock not advised, continue cpr");
        }

    }
}


void MainWindow::shock(){
    if (battery >= 10) {
        ui->compressionButton->setEnabled(true);
        ui->breathButton->setEnabled(true);
        ui->shockButton->setEnabled(false);
        updateTextbox("AED Audio: Shock delivered, continue cpr");
        battery = battery - 10;
    } else {
        updateTextbox("AED Audio: Battery low, shock not delivered");
        ui->compressionButton->setEnabled(true);
        ui->breathButton->setEnabled(true);
        ui->shockButton->setEnabled(false);
    }
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
            qDebug() << uiWidget;
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



