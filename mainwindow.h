#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <aed.h>
#include <patient.h>
#include <QTimer>
#include <QString>
#include <string>
#include <cstring>

#define NUM_COMPRESSIONS 4
#define NUM_BREATHS 2
#define GOOD_CPR_PERCENT 0.2
#define OK_CPR_PERCENT 0.1
#define MAX_HEART_RATE 600

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTimer emsTimer; //Timer that envokes the EMS end state after 2-5 minutes
    QTimer minuteCounter; //Timer that updates the user on how long until EMS arrives (Updates every second)
    bool emsArrived;
    int numCompressions;
    int cprQuality; //Quality of CPR that ranges from 0-3
    QString cprString; //Stores the pattern of compressions and breaths in one CPR cycle
    QString idealPattern; //Stores the ideal pattern of compressions and breaths in one CPR cycle
    AED* aed;
    Patient* patient;
    Ui::MainWindow *ui;
    void setHighButtons(); //Helper function to set proper buttons that should appear if heart rate is high
    void setNormalButtons(); //Helper function to set proper buttons that should appear if heart rate is within normal range
    void hideAll();
    void showAll();

private slots:
    void reset();
    void beginSimulation();
    void powerOn();
    void powerOff();
    void checkResponse();
    void callEMS();
    void openAirways();
    void checkBreathing();
    void clearChest();
    void applyPads();
    void moveAway();
    void shock();
    void performCPR();
    void emsArrives();
    void updateFromOther();
    void disconnectElectrode();

};
#endif // MAINWINDOW_H
