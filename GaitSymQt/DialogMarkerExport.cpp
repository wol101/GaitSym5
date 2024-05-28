#include "DialogMarkerExport.h"
#include "ui_DialogMarkerExport.h"

#include "Preferences.h"
#include "DataFile.h"
#include "GSUtil.h"
#include "Marker.h"
#include "Simulation.h"
#include "Body.h"
#include "LineEditPath.h"

#include "pystring.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>

using namespace std::string_literals;

DialogMarkerExport::DialogMarkerExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMarkerExport)
{
    ui->setupUi(this);

    setWindowTitle(tr("Marker Import/Export"));
#ifdef Q_OS_MACOS
    setWindowFlags(windowFlags() & (~Qt::Dialog) |
                   Qt::Window); // allows the window to be resized on macs
#endif

    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &DialogMarkerExport::reject);
    connect(ui->pushButtonProcess, &QAbstractButton::clicked, this, &DialogMarkerExport::processButtonClicked);
    connect(ui->lineEditFileName, &QLineEdit::textChanged, this, &DialogMarkerExport::fileNameTextChanged);

    ui->lineEditFileName->setPathType(LineEditPath::FileForSave);

    SetUIElementsFromPreferences();

    EnableWidgets();

    restoreGeometry(Preferences::valueQByteArray("DialogMarkerExportGeometry"));
}

DialogMarkerExport::~DialogMarkerExport()
{
    delete ui;
}

void DialogMarkerExport::processButtonClicked() // this catches OK and return/enter
{
    qDebug() << "DialogMarkerExportGeometry::applyButtonClicked()";
    ExportMarkers();
    SaveUIElementsToPreferences();
    Preferences::insert("DialogMarkerExportGeometry", saveGeometry());
}

void DialogMarkerExport::reject() // this catches cancel, close and escape key
{
    qDebug() << "DialogGeoms::reject()";
    Preferences::insert("DialogMarkerExportGeometry", saveGeometry());
    QDialog::reject();
}

void DialogMarkerExport::closeEvent(QCloseEvent *event)
{
    qDebug() << "DialogMarkerExportGeometry::closeEvent()";
    SaveUIElementsToPreferences();
    Preferences::insert("DialogMarkerExportGeometry", saveGeometry());
    QDialog::closeEvent(event);
}

void DialogMarkerExport::setSimulation(GaitSym::Simulation *simulation)
{
    m_simulation = simulation;
}

void DialogMarkerExport::SetUIElementsFromPreferences()
{
    int formatOption = Preferences::valueInt("DialogMarkerExportFormatOption");
    switch (formatOption)
    {
    case 0:
        ui->radioButtonPositionOnly->setChecked(true);
        break;
    case 1:
        ui->radioButtonPositionEuler->setChecked(true);
        break;
    case 2:
        ui->radioButtonPositionAngleAxis->setChecked(true);
        break;
    case 3:
        ui->radioButtonPositionQuaternion->setChecked(true);
        break;
    case 4:
        ui->radioButtonPositionMatrix->setChecked(true);
        break;
    }
    int separatorOption = Preferences::valueInt("DialogMarkerExportSeparatorOption");
    switch (separatorOption)
    {
    case 0:
        ui->radioButtonCommaSeparator->setChecked(true);
        break;
    case 1:
        ui->radioButtonTabSeparator->setChecked(true);
        break;
    case 2:
        ui->radioButtonWhiteSpaceSeparator->setChecked(true);
        break;
    }
    ui->checkBoxHeaderRow->setChecked(Preferences::valueBool("DialogMarkerExportHeaderRow"));
    ui->checkBoxQuotedStrings->setChecked(Preferences::valueBool("DialogMarkerExportQuotedStrings"));
    ui->checkBoxAnglesInRadians->setChecked(Preferences::valueBool("DialogMarkerImportAnglesInRadians"));
    ui->checkBoxBodyLocalCoordinates->setChecked(Preferences::valueBool("DialogMarkerIgnoreBodyLocalCoordinates"));

    ui->lineEditFileName->setText(Preferences::valueQString("DialogMarkerExportFileName"));
}

void DialogMarkerExport::SaveUIElementsToPreferences()
{
    int formatOption = 0;
    if (ui->radioButtonPositionOnly->isChecked()) formatOption = 0;
    if (ui->radioButtonPositionEuler->isChecked()) formatOption = 1;
    if (ui->radioButtonPositionAngleAxis->isChecked()) formatOption = 2;
    if (ui->radioButtonPositionQuaternion->isChecked()) formatOption = 3;
    if (ui->radioButtonPositionMatrix->isChecked()) formatOption = 4;
    Preferences::insert("DialogMarkerExportFormatOption", formatOption);

    int separatorOption = 0;
    if (ui->radioButtonCommaSeparator->isChecked()) separatorOption = 0;
    if (ui->radioButtonTabSeparator->isChecked()) separatorOption = 1;
    if (ui->radioButtonWhiteSpaceSeparator->isChecked()) separatorOption = 2;
    Preferences::insert("DialogMarkerExportSeparatorOption", separatorOption);

    Preferences::insert("DialogMarkerExportHeaderRow", ui->checkBoxHeaderRow->isChecked());
    Preferences::insert("DialogMarkerExportQuotedStrings", ui->checkBoxQuotedStrings->isChecked());
    Preferences::insert("DialogMarkerImportAnglesInRadians", ui->checkBoxAnglesInRadians->isChecked());
    Preferences::insert("DialogMarkerIgnoreBodyLocalCoordinates", ui->checkBoxBodyLocalCoordinates->isChecked());

    Preferences::insert("DialogMarkerExportFileName", ui->lineEditFileName->text());
}

int DialogMarkerExport::ExportMarkers()
{
    std::vector<std::string> lines;
    pgd::Vector3 pOutput;
    pgd::Quaternion qOutput;
    std::string bodyName;

    // build the delimiter character
    std::string separator = " "s;
    if (ui->radioButtonCommaSeparator->isChecked()) separator = ","s;
    if (ui->radioButtonTabSeparator->isChecked()) separator = "\t"s;

    if (ui->radioButtonPositionOnly->isChecked())
    {
        if (ui->checkBoxHeaderRow->isChecked())
        {
            std::vector<std::string> header = {"Name"s, "Body"s, "X"s, "Y"s, "Z"s};
            lines.push_back(pystring::join(separator, header));
        }
        for (auto &&markerIt : *m_simulation->GetMarkerList())
        {
            if (markerIt.second->GetBody()) bodyName = markerIt.second->GetBody()->name();
            else bodyName = "World"s;
            ui->plainTextEditLog->appendPlainText(QString("Processing marker '%1\n").arg(QString::fromStdString(bodyName)));
            std::vector<std::string> line;
            line.reserve(5);
            line.push_back(markerIt.first);
            line.push_back(bodyName);
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { pOutput = markerIt.second->GetPosition(); }
            else { pOutput = markerIt.second->GetWorldPosition(); }
            line.push_back(GaitSym::GSUtil::ToString(pOutput.x));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.y));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.z));
            lines.push_back(pystring::join(separator, line));
        }
    }
    else if (ui->radioButtonPositionEuler->isChecked())
    {
        if (ui->checkBoxHeaderRow->isChecked())
        {
            std::vector<std::string> header = {"Name"s, "Body"s, "X"s, "Y"s, "Z"s, "RX"s, "RY"s, "RZ"s};
            lines.push_back(pystring::join(separator, header));
        }
        for (auto &&markerIt : *m_simulation->GetMarkerList())
        {
            if (markerIt.second->GetBody()) bodyName = markerIt.second->GetBody()->name();
            else bodyName = "World"s;
            ui->plainTextEditLog->appendPlainText(QString("Processing marker '%1\n").arg(QString::fromStdString(bodyName)));
            std::vector<std::string> line;
            line.reserve(5);
            line.push_back(markerIt.first);
            line.push_back(bodyName);
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { pOutput = markerIt.second->GetPosition(); }
            else { pOutput = markerIt.second->GetWorldPosition(); }
            line.push_back(GaitSym::GSUtil::ToString(pOutput.x));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.y));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.z));
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { qOutput = markerIt.second->GetQuaternion(); }
            else { qOutput = markerIt.second->GetWorldQuaternion(); }
            pgd::Vector3 euler;
            if (ui->checkBoxAnglesInRadians->isChecked()) euler = pgd::MakeEulerAnglesFromQRadian(qOutput);
            else euler = pgd::MakeEulerAnglesFromQ(qOutput);
            line.push_back(GaitSym::GSUtil::ToString(euler.x));
            line.push_back(GaitSym::GSUtil::ToString(euler.y));
            line.push_back(GaitSym::GSUtil::ToString(euler.z));
            lines.push_back(pystring::join(separator, line));
        }
    }
    else if (ui->radioButtonPositionAngleAxis->isChecked())
    {
        if (ui->checkBoxHeaderRow->isChecked())
        {
            std::vector<std::string> header = {"Name"s, "Body"s, "X"s, "Y"s, "Z"s, "R"s, "AX"s, "AY"s, "AZ"s};
            lines.push_back(pystring::join(separator, header));
        }
        for (auto &&markerIt : *m_simulation->GetMarkerList())
        {
            if (markerIt.second->GetBody()) bodyName = markerIt.second->GetBody()->name();
            else bodyName = "World"s;
            ui->plainTextEditLog->appendPlainText(QString("Processing marker '%1\n").arg(QString::fromStdString(bodyName)));
            std::vector<std::string> line;
            line.reserve(5);
            line.push_back(markerIt.first);
            line.push_back(bodyName);
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { pOutput = markerIt.second->GetPosition(); }
            else { pOutput = markerIt.second->GetWorldPosition(); }
            line.push_back(GaitSym::GSUtil::ToString(pOutput.x));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.y));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.z));
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { qOutput = markerIt.second->GetQuaternion(); }
            else { qOutput = markerIt.second->GetWorldQuaternion(); }
            double xa, ya, za, angle;
            pgd::MakeAxisAngleFromQ(qOutput, &xa, &ya, &za, &angle);
            if (ui->checkBoxAnglesInRadians->isChecked() == false) angle = pgd::RadToDeg(angle);
            line.push_back(GaitSym::GSUtil::ToString(angle));
            line.push_back(GaitSym::GSUtil::ToString(xa));
            line.push_back(GaitSym::GSUtil::ToString(ya));
            line.push_back(GaitSym::GSUtil::ToString(za));
            lines.push_back(pystring::join(separator, line));
        }
    }
    else if (ui->radioButtonPositionQuaternion->isChecked())
    {
        if (ui->checkBoxHeaderRow->isChecked())
        {
            std::vector<std::string> header = {"Name"s, "Body"s, "X"s, "Y"s, "Z"s, "QN"s, "QX"s, "QY"s, "QZ"s};
            lines.push_back(pystring::join(separator, header));
        }
        for (auto &&markerIt : *m_simulation->GetMarkerList())
        {
            if (markerIt.second->GetBody()) bodyName = markerIt.second->GetBody()->name();
            else bodyName = "World"s;
            ui->plainTextEditLog->appendPlainText(QString("Processing marker '%1\n").arg(QString::fromStdString(bodyName)));
            std::vector<std::string> line;
            line.reserve(5);
            line.push_back(markerIt.first);
            line.push_back(bodyName);
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { pOutput = markerIt.second->GetPosition(); }
            else { pOutput = markerIt.second->GetWorldPosition(); }
            line.push_back(GaitSym::GSUtil::ToString(pOutput.x));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.y));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.z));
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { qOutput = markerIt.second->GetQuaternion(); }
            else { qOutput = markerIt.second->GetWorldQuaternion(); }
            line.push_back(GaitSym::GSUtil::ToString(qOutput.n));
            line.push_back(GaitSym::GSUtil::ToString(qOutput.x));
            line.push_back(GaitSym::GSUtil::ToString(qOutput.y));
            line.push_back(GaitSym::GSUtil::ToString(qOutput.z));
            lines.push_back(pystring::join(separator, line));
        }
    }
    else if (ui->radioButtonPositionMatrix->isChecked())
    {
        if (ui->checkBoxHeaderRow->isChecked())
        {
            std::vector<std::string> header = {"Name"s, "Body"s, "X"s, "Y"s, "Z"s, "R1C1"s, "R1C2"s, "R1C3"s, "R2C1"s, "R2C2"s, "R2C3"s, "R3C1"s, "R3C2"s, "R3C3"s};
            lines.push_back(pystring::join(separator, header));
        }
        for (auto &&markerIt : *m_simulation->GetMarkerList())
        {
            if (markerIt.second->GetBody()) bodyName = markerIt.second->GetBody()->name();
            else bodyName = "World"s;
            ui->plainTextEditLog->appendPlainText(QString("Processing marker '%1\n").arg(QString::fromStdString(bodyName)));
            std::vector<std::string> line;
            line.reserve(5);
            line.push_back(markerIt.first);
            line.push_back(bodyName);
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { pOutput = markerIt.second->GetPosition(); }
            else { pOutput = markerIt.second->GetWorldPosition(); }
            line.push_back(GaitSym::GSUtil::ToString(pOutput.x));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.y));
            line.push_back(GaitSym::GSUtil::ToString(pOutput.z));
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { qOutput = markerIt.second->GetQuaternion(); }
            else { qOutput = markerIt.second->GetWorldQuaternion(); }
            pgd::Matrix3x3 matrix = MakeMFromQ(qOutput);
            for (size_t i =0; i < 9; i++)
                line.push_back(GaitSym::GSUtil::ToString(matrix.data()[i]));
            lines.push_back(pystring::join(separator, line));
        }
    }

    std::string data = pystring::join("\n"s, lines);
    GaitSym::DataFile dataFile;
    dataFile.SetRawData(data.data(), data.size());
    QString fileName = ui->lineEditFileName->text();
    if (dataFile.WriteFile(fileName.toStdString()))
    {
        ui->plainTextEditLog->appendPlainText(QString("Error writing '%1'.\n").arg(fileName));
        QMessageBox::information(this, "Marker Export Errors", QString("Error writing '%1'.\n").arg(fileName), QMessageBox::Ok);
        return __LINE__;
    }

    ui->plainTextEditLog->appendPlainText(QString("%1 markers written to '%2'.\n").arg(m_simulation->GetMarkerList()->size()).arg(fileName));
    // QMessageBox::information(this, "Marker Export Results", QString("%1 markers written to '%2'.\n").arg(m_simulation->GetMarkerList()->size()).arg(fileName), QMessageBox::Ok);
    return 0;
}


void DialogMarkerExport::EnableWidgets()
{
    QString folder = QFileInfo(ui->lineEditFileName->text()).path();
    bool folderAvailable = QFileInfo(folder).isDir();
    ui->pushButtonProcess->setEnabled(folderAvailable);
}

void DialogMarkerExport::fileNameTextChanged(const QString &text)
{
    EnableWidgets();
}
