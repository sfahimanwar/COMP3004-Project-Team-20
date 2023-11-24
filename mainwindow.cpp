#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->aedDisplay->setPixmap(QPixmap(":/resources/img/aed.jpg"));

    //Can Set either Patient or Child Image using the code below
    /*
    ui->patientLabel->setPixmap(QPixmap(":/resources/img/child.jpg"));
    ui->patientLabel->setPixmap(QPixmap(":/resources/img/dummy.jpg"));
    */

    //Can set ECG image using the code below
    /*
    ui->ecgLabel->setPixmap(QPixmap(":/resources/img/normal.jpg"));
    ui->ecgLabel->setPixmap(QPixmap(":/resources/img/fast.jpg"));
    ui->ecgLabel->setPixmap(QPixmap(":/resources/img/slow.jpg"));
    ui->ecgLabel->setPixmap(QPixmap(":/resources/img/irregular.jpg"));
    */

}

MainWindow::~MainWindow()
{
    delete ui;
}

