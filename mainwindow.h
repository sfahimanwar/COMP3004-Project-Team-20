#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <aed.h>
#include <patient.h>
#include <QListWidgetItem>
#include <QtCharts>
#include <QChartView>
#include <QLineSeries>
#include <string>
#include <QStack>



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
    int EMSTimer;
    int numCompressions;
    int CPRQuality;
    bool isTouching;
    AED* aed;
    Patient* patient;
    Ui::MainWindow *ui;
    void updateTextbox(QString);
    QChart *chart;
    QLineSeries *series;
    QChartView *chartView;
    void initializeGraph();

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

};
#endif // MAINWINDOW_H
