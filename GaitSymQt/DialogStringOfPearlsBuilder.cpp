#include "DialogStringOfPearlsBuilder.h"
#include "ui_DialogStringOfPearlsBuilder.h"

DialogStringOfPearlsBuilder::DialogStringOfPearlsBuilder(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogStringOfPearlsBuilder)
{
    ui->setupUi(this);
}

DialogStringOfPearlsBuilder::~DialogStringOfPearlsBuilder()
{
    delete ui;
}
