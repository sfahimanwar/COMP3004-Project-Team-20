#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <aed.h>
#include <patient.h>
#include <QTimer>
#include <string>
#include <cstring>

#define NUM_COMPRESSIONS 2
#define NUM_BREATHS 1
#define GOOD_CPR_PERCENT 0.2
#define OK_CPR_PERCENT 0.1

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
    QTimer emsTimer;
    QTimer minuteCounter;
    bool emsArrived;
    int numCompressions;
    int CPRQuality;
    bool isTouching;
    std::string cprString = "";
    int compressionsCount = 0;
    int breathsCount = 0;
    AED* aed;
    Patient* patient;
    Ui::MainWindow *ui;
    int battery;
    void updateTextbox(QString);
    void updateECG();
    void setHighButtons(); //Helper function to set proper buttons that should appear if heart rate is high
    void setNormalButtons(); //Helper function to set proper buttons that should appear if heart rate is within normal range
    void hideAll();
    void showAll();
    bool selfCheck();

private slots:
    void reset();
    void beginSimulation();
    void powerOn();
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

};
#endif // MAINWINDOW_H
