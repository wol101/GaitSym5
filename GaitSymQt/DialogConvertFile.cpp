#include "DialogConvertFile.h"
#include "ui_DialogConvertFile.h"

#include "Preferences.h"
#include "ParseXML.h"
#include "Simulation.h"
#include "MainWindow.h"

#include <QPushButton>
#include <QValidator>
#include <QFileDialog>
#include <QLayout>
#include <QMessageBox>

#include <fstream>

using namespace std::string_literals;

DialogConvertFile::DialogConvertFile(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogConvertFile)
{
    ui->setupUi(this);

    setWindowTitle(tr("Convert File"));
#ifdef Q_OS_MACOS
    setWindowFlags(windowFlags() & (~Qt::Dialog) | Qt::Window); // allows the window to be resized on macs
#endif
    connect(ui->pushButtonClose, &QPushButton::clicked, this, &DialogConvertFile::close);
    connect(ui->pushButtonInputFile, &QPushButton::clicked, this, &DialogConvertFile::pushButtonInputFileClicked);
    connect(ui->pushButtonOutputFile, &QPushButton::clicked, this, &DialogConvertFile::pushButtonOutputFileClicked);
    connect(ui->pushButtonConvert, &QPushButton::clicked, this, &DialogConvertFile::pushButtonConvertClicked);
    connect(ui->lineEditInputFile, &QLineEdit::textChanged, this, &DialogConvertFile::lineEditTextChanged);
    connect(ui->lineEditOutputFile, &QLineEdit::textChanged, this, &DialogConvertFile::lineEditTextChanged);

    ui->lineEditInputFile->setPathType(LineEditPath::FileForOpen);
    ui->lineEditOutputFile->setPathType(LineEditPath::FileForSave);

    MainWindow::layoutSpacing(this);

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

void DialogConvertFile::enableWidgets()
{
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
    log(QString("Reading \"%1\"").arg(ui->lineEditInputFile->text()));
    std::ostringstream buffer;
    try {
        std::ifstream file(ui->lineEditInputFile->text().toStdString(), std::ios::binary);
        if (!file) return;
        buffer << file.rdbuf();
        file.close();
    }
    catch (...)
    {
        log(QString("Error reading \"%1\"").arg(ui->lineEditInputFile->text()));
        return;
    }
    std::string str = buffer.str();
    std::string rootNodeTag;
    GaitSym::ParseXML parseXML;
    std::string *err = parseXML.LoadModel(str.data(), str.size(), &rootNodeTag);
    if (err) { log(QString("Error parsing \"%1\"\n%1").arg(ui->lineEditInputFile->text()).arg(QString::fromStdString(*err))); return; }
    else { log(QString("\"%1\" parsed successfully").arg(ui->lineEditInputFile->text())); }

    while (true)
    {
        if (rootNodeTag == "GAITSYM5"s)
        {
            log(QString("File type is \"%1\". No conversion necessary.").arg(QString::fromStdString(rootNodeTag)));
            break;
        }

        if (rootNodeTag == "GAITSYM2019"s)
        {
            log(QString("File type is \"%1\". Converting.").arg(QString::fromStdString(rootNodeTag)));

            // create a new xml file
            std::string newRootNodeTag("GAITSYM5"s);
            std::string xml = parseXML.SaveModel(newRootNodeTag, "File generated using DialogConvertFile"s);
            GaitSym::ParseXML newParseXML;
            err = newParseXML.LoadModel(xml.c_str(), xml.size(), &newRootNodeTag);
            if (err) { QMessageBox::warning(this, "Internal XML parse error", QString("'%1'").arg(QString::fromStdString(*err))); return; } // this should never happen
            auto elementList = newParseXML.elementList();
            for (auto &&tagElementIt : *elementList)
            {
                if (tagElementIt->tag == "GLOBAL"s)
                {
                    tagElementIt->attributes["PhysicsEngine"s] = "ODE"s;
                    log(QString("Adding PhysicsEngine=\"ODE\""));
                    std::string fitnessType("TargetSum"s);
                    if (tagElementIt->attributes["FitnessType"] == "KinematicMatchMiniMax"s) { fitnessType = "TargetMaxiMin"s; }
                    tagElementIt->attributes["FitnessType"] = fitnessType;
                    log(QString("Adding FitnessType=\"%1\"").arg(QString::fromStdString(fitnessType)));
                }
            }
            log(QString("Adding default lights"));
            QFile lightsFile(":/templates/LightsTemplate.xml");
            if (!lightsFile.open(QIODevice::ReadOnly)) { QMessageBox::warning(this, "Internal XML parse error", QString("Unable to open resource file: \"%1\"").arg(lightsFile.fileName())); return; } // this should never happen
            QByteArray lightsData = lightsFile.readAll();
            GaitSym::ParseXML lightsXML;
            err = lightsXML.LoadModel(lightsData.data(), lightsData.size(), nullptr);
            if (err) { QMessageBox::warning(this, "LightsTemplate.xml: Internal XML parse error", QString("'%1'").arg(QString::fromStdString(*err))); return; } // this should never happen
            for (auto &&elementIt : *lightsXML.elementList()) { elementList->push_back(std::move(elementIt)); }
            xml = newParseXML.SaveModel(newRootNodeTag, "File generated using DialogConvertFile"s);
            log(QString("Writing \"%1\"").arg(ui->lineEditOutputFile->text()));
            try {
                std::ofstream file(ui->lineEditOutputFile->text().toStdString(), std::ios::binary);
                if (!file) return;
                file << xml;
                file.close();
            }
            catch (...)
            {
                log(QString("Error writing \"%1\"").arg(ui->lineEditOutputFile->text()));
                return;
            }
            GaitSym::Simulation simulation;
            err = simulation.LoadModel(xml.data(), xml.size());
            if (err)
            {
                log(QString("WARNING\nWARNING: \"%1\" converted but fails validation.\nWARNING%s").arg(ui->lineEditOutputFile->text()).arg(QString::fromStdString(*err)));
            }
            break;
        }


        log(QString("File type is \"%1\". Unable to convert.").arg(QString::fromStdString(rootNodeTag)));
        break;
    }
}

void DialogConvertFile::log(const QString &text)
{
    ui->plainTextEdit->appendPlainText(text);
    ui->plainTextEdit->repaint();
}

