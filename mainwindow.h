#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <aed.h>
#include <patient.h>
#include <QTimer>

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
    AED* aed;
    Patient* patient;
    Ui::MainWindow *ui;
    void updateTextbox(QString);
    void hideAll();

private slots:
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

};
#endif // MAINWINDOW_H
