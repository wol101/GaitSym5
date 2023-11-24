#include "mainwindow.h"
#include "ui_mainwindow.h"

extern int snippetMain(int, const char*const*);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    snippetMain(0, 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

