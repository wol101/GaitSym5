#include "DialogConvertFile.h"
#include "ui_DialogConvertFile.h"

#include "Preferences.h"
#include "ParseXML.h"

#include <QPushButton>
#include <QValidator>
#include <QFileDialog>

#include <fstream>

DialogConvertFile::DialogConvertFile(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogConvertFile)
{
    ui->setupUi(this);

    setWindowTitle(tr("Convert File"));
#ifdef Q_OS_MACOS
    setWindowFlags(windowFlags() & (~Qt::Dialog) | Qt::Window); // allows the window to be resized on macs
#endif
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &DialogConvertFile::accept);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &DialogConvertFile::accept);
    connect(ui->pushButtonInputFile, &QPushButton::clicked, this, &DialogConvertFile::pushButtonInputFileClicked);
    connect(ui->pushButtonOutputFile, &QPushButton::clicked, this, &DialogConvertFile::pushButtonOutputFileClicked);
    connect(ui->pushButtonConvert, &QPushButton::clicked, this, &DialogConvertFile::pushButtonConvertClicked);
    connect(ui->lineEditInputFile, &QLineEdit::textChanged, this, &DialogConvertFile::lineEditTextChanged);
    connect(ui->lineEditOutputFile, &QLineEdit::textChanged, this, &DialogConvertFile::lineEditTextChanged);

    ui->lineEditInputFile->setPathType(LineEditPath::FileForOpen);
    ui->lineEditOutputFile->setPathType(LineEditPath::FileForSave);

    readSettings();
}

DialogConvertFile::~DialogConvertFile()
{
    delete ui;
}

// note: need to trap closeEvent, accept and reject to be sure of getting all the things that might dismiss the dialog

void DialogConvertFile::closeEvent(QCloseEvent *event)
{
    writeSettings();
    QDialog::closeEvent(event);
}

void DialogConvertFile::accept() // this catches OK button
{
    doConversion();
    writeSettings();
    QDialog::accept();
}

void DialogConvertFile::reject() // this catches cancel, close and escape key
{
    writeSettings();
    QDialog::reject();
}

void DialogConvertFile::enableWidgets()
{
    ui->pushButtonOK->setEnabled(ui->lineEditInputFile->state() == QValidator::Acceptable && ui->lineEditOutputFile->state() == QValidator::Acceptable);
    ui->pushButtonConvert->setEnabled(ui->lineEditInputFile->state() == QValidator::Acceptable && ui->lineEditOutputFile->state() == QValidator::Acceptable);
}

void DialogConvertFile::readSettings()
{
    ui->lineEditInputFile->setText(Preferences::valueQString("DialogConvertFileInputFile", ""));
    ui->lineEditOutputFile->setText(Preferences::valueQString("DialogConvertFileOutputFile", ""));
    restoreGeometry(Preferences::valueQByteArray("DialogConvertFileGeometry"));
}

void DialogConvertFile::writeSettings()
{
    Preferences::insert("DialogConvertFileGeometry", saveGeometry());
    Preferences::insert("DialogConvertFileInputFile", ui->lineEditInputFile->text());
    Preferences::insert("DialogConvertFileOutputFile", ui->lineEditOutputFile->text());
}

void DialogConvertFile::pushButtonInputFileClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "File to convert", ui->lineEditInputFile->text(), "GaitSym Files (*.gaitsym *.xml);;Any File (*.* *)");
    if (filename.size())
    {
        ui->lineEditInputFile->setText(filename);
    }
}

void DialogConvertFile::pushButtonOutputFileClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Converted file to save", ui->lineEditOutputFile->text(), "GaitSym Files (*.gaitsym *.xml);;Any File (*.* *)");
    if (filename.size())
    {
        ui->lineEditOutputFile->setText(filename);
    }
}

void DialogConvertFile::pushButtonConvertClicked()
{
    doConversion();
}

void DialogConvertFile::lineEditTextChanged(const QString &/*text*/)
{
    enableWidgets();
}

void DialogConvertFile::doConversion()
{
    std::ostringstream buffer;
    try {
        std::ifstream file(ui->lineEditInputFile->text().toStdString(), std::ios::binary);
        if (!file) return;
        buffer << file.rdbuf();
        file.close();
    }
    catch (...)
    {
        return;
    }
    std::string str = buffer.str();
    std::string rootNodeTag;
    GaitSym::ParseXML parseXML;
    std::string *err = parseXML.LoadModel(str.data(), str.size(), &rootNodeTag);
}
