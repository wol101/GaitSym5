#include "DialogMarkerImportExport.h"
#include "ui_DialogMarkerImportExport.h"

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

using namespace std::string_literals;

DialogMarkerImportExport::DialogMarkerImportExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMarkerImportExport)
{
    ui->setupUi(this);

    setWindowTitle(tr("Marker Import/Export"));
#ifdef Q_OS_MACOS
    setWindowFlags(windowFlags() & (~Qt::Dialog) |
                   Qt::Window); // allows the window to be resized on macs
#endif

    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogMarkerImportExport::accept);
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &DialogMarkerImportExport::reject);
    connect(ui->radioButtonExport, &QAbstractButton::clicked, this, &DialogMarkerImportExport::radioButtonExportClicked);
    connect(ui->radioButtonImport, &QAbstractButton::clicked, this, &DialogMarkerImportExport::radioButtonImportClicked);

    ui->lineEditFileName->setPathType(LineEditPath::FileForOpen);

    SetUIElementsFromPreferences();

    restoreGeometry(Preferences::valueQByteArray("DialogMarkerImportExportGeometry"));
}

DialogMarkerImportExport::~DialogMarkerImportExport()
{
    delete ui;
}

void DialogMarkerImportExport::accept() // this catches OK and return/enter
{
    qDebug() << "DialogMarkerImportExportGeometry::accept()";

    if (ui->radioButtonImport->isChecked()) ImportMarkers();
    else if (ui->radioButtonExport->isChecked()) ExportMarkers();

    SaveUIElementsToPreferences();
    Preferences::insert("DialogMarkerImportExportGeometry", saveGeometry());
    QDialog::accept();
}

void DialogMarkerImportExport::reject() // this catches cancel, close and escape key
{
    qDebug() << "DialogMarkerImportExportGeometry::reject()";
    SaveUIElementsToPreferences();
    Preferences::insert("DialogMarkerImportExportGeometry", saveGeometry());
    QDialog::reject();
}

void DialogMarkerImportExport::closeEvent(QCloseEvent *event)
{
    qDebug() << "DialogMarkerImportExportGeometry::closeEvent()";
    SaveUIElementsToPreferences();
    Preferences::insert("DialogMarkerImportExportGeometry", saveGeometry());
    QDialog::closeEvent(event);
}

void DialogMarkerImportExport::setAllowImport(bool newAllowImport)
{
    m_allowImport = newAllowImport;
    if (m_allowImport)
    {
        ui->radioButtonExport->setEnabled(true);
        ui->radioButtonExport->setChecked(false);
        ui->radioButtonImport->setEnabled(true);
        ui->radioButtonImport->setChecked(true);
    }
    else
    {
        ui->radioButtonExport->setEnabled(true);
        ui->radioButtonExport->setChecked(true);
        ui->radioButtonImport->setEnabled(false);
        ui->radioButtonImport->setChecked(false);
    }
}

std::vector<std::unique_ptr<GaitSym::Marker> > *DialogMarkerImportExport::markerList() const
{
    return m_markerList;
}

void DialogMarkerImportExport::radioButtonExportClicked()
{
    ui->lineEditFileName->setPathType(LineEditPath::FileForSave);
}

void DialogMarkerImportExport::radioButtonImportClicked()
{
    ui->lineEditFileName->setPathType(LineEditPath::FileForOpen);
}

void DialogMarkerImportExport::setMarkerList(std::vector<std::unique_ptr<GaitSym::Marker> > *markerList)
{
    m_markerList = markerList;
}

void DialogMarkerImportExport::setSimulation(GaitSym::Simulation *simulation)
{
    m_simulation = simulation;
}

void DialogMarkerImportExport::SetUIElementsFromPreferences()
{
    int formatOption = Preferences::valueInt("DialogMarkerImportExportFormatOption");
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
    int separatorOption = Preferences::valueInt("DialogMarkerImportExportSeparatorOption");
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
    int importExport = Preferences::valueInt("DialogMarkerImportExportImportExport");
    switch (importExport)
    {
    case 0:
        ui->radioButtonImport->setChecked(true);
        break;
    case 1:
        ui->radioButtonExport->setChecked(true);
        break;
    }
    ui->checkBoxHeaderRow->setChecked(Preferences::valueBool("DialogMarkerImportExportHeaderRow"));
    ui->checkBoxQuotedStrings->setChecked(Preferences::valueBool("DialogMarkerImportExportQuotedStrings"));
    ui->checkBoxAnglesInRadians->setChecked(Preferences::valueBool("DialogMarkerImportAnglesInRadians"));
    ui->checkBoxAllowOverwrite->setChecked(Preferences::valueBool("DialogMarkerImportAllowOverwrite"));
    ui->checkBoxIgnoreMissingBodies->setChecked(Preferences::valueBool("DialogMarkerIgnoreMissingBodies"));
    ui->checkBoxBodyLocalCoordinates->setChecked(Preferences::valueBool("DialogMarkerIgnoreBodyLocalCoordinates"));

    ui->lineEditFileName->setText(Preferences::valueQString("DialogMarkerImportExportFileName"));
}

void DialogMarkerImportExport::SaveUIElementsToPreferences()
{
    int formatOption = 0;
    if (ui->radioButtonPositionOnly->isChecked()) formatOption = 0;
    if (ui->radioButtonPositionEuler->isChecked()) formatOption = 1;
    if (ui->radioButtonPositionAngleAxis->isChecked()) formatOption = 2;
    if (ui->radioButtonPositionQuaternion->isChecked()) formatOption = 3;
    if (ui->radioButtonPositionMatrix->isChecked()) formatOption = 4;
    Preferences::insert("DialogMarkerImportExportFormatOption", formatOption);

    int separatorOption = 0;
    if (ui->radioButtonCommaSeparator->isChecked()) separatorOption = 0;
    if (ui->radioButtonTabSeparator->isChecked()) separatorOption = 1;
    if (ui->radioButtonWhiteSpaceSeparator->isChecked()) separatorOption = 2;
    Preferences::insert("DialogMarkerImportExportSeparatorOption", separatorOption);

    int importExport = 0;
    if (ui->radioButtonImport->isChecked()) importExport = 0;
    if (ui->radioButtonExport->isChecked()) importExport = 1;
    Preferences::insert("DialogMarkerImportExportImportExport", importExport);

    Preferences::insert("DialogMarkerImportExportHeaderRow", ui->checkBoxHeaderRow->isChecked());
    Preferences::insert("DialogMarkerImportExportQuotedStrings", ui->checkBoxQuotedStrings->isChecked());
    Preferences::insert("DialogMarkerImportAnglesInRadians", ui->checkBoxAnglesInRadians->isChecked());
    Preferences::insert("DialogMarkerImportAllowOverwrite", ui->checkBoxAllowOverwrite->isChecked());
    Preferences::insert("DialogMarkerIgnoreMissingBodies", ui->checkBoxIgnoreMissingBodies->isChecked());
    Preferences::insert("DialogMarkerIgnoreBodyLocalCoordinates", ui->checkBoxBodyLocalCoordinates->isChecked());

    Preferences::insert("DialogMarkerImportExportFileName", ui->lineEditFileName->text());
}

int DialogMarkerImportExport::ExportMarkers()
{
    std::vector<std::string> lines;
    pgd::Vector3 pOutput;
    pgd::Quaternion qOutput;
    std::string bodyName;

    m_markerList = nullptr; // this is simply used to inform the calling program that m_markerList has not been used.

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
    QMessageBox::information(this, "Marker Export Results", QString("%1 markers written to '%2'.\n").arg(m_simulation->GetMarkerList()->size()).arg(fileName), QMessageBox::Ok);
    return 0;
}

int DialogMarkerImportExport::ImportMarkers()
{
    Q_ASSERT_X(m_markerList, "DialogMarkerImportExport::ImportMarkers", "m_markerList not defined");
    size_t errorCount = 0;
    QString fileName = ui->lineEditFileName->text();
    GaitSym::DataFile dataFile;
    if (dataFile.ReadFile(fileName.toStdString()))
    {
        errorCount++;
        ui->plainTextEditLog->appendPlainText(QString("Error: Could not read '%1'. No markers imported.\n").arg(fileName));
        QMessageBox::warning(this, "Marker Import Error", QString("Could not read '%1'\nNo markers imported.").arg(fileName), QMessageBox::Ok);
        return __LINE__;
    }

    // build the delimiter character
    char separator = 0;
    if (ui->radioButtonCommaSeparator->isChecked()) separator = ',';
    if (ui->radioButtonTabSeparator->isChecked()) separator = '\t';

    bool quoted = ui->checkBoxQuotedStrings->isChecked();
    bool allowEmpty = false;

    pgd::Quaternion qInput;
    double angle;
    std::vector<std::string> lines;
    std::vector<std::string> tokens;
    std::set<std::string> addedNames;
    std::vector<double> values;
    values.reserve(12);
    pystring::splitlines(std::string(dataFile.GetRawData(), dataFile.GetSize()), lines);
    size_t startLine = 0;
    if (ui->checkBoxHeaderRow->isChecked())
    {
        ui->plainTextEditLog->appendPlainText(QString("Skipping header line.\n%1\n").arg(QString::fromStdString(lines[0])));
        startLine++;
    }
    for (size_t i = startLine; i < lines.size(); i++)
    {
        if (GaitSym::GSUtil::SplitGeneric(lines[i], &tokens, separator, quoted, allowEmpty) < 2) continue;
        if (!ui->checkBoxAllowOverwrite->isChecked() && m_simulation->GetMarker(tokens[0]))
        {
            errorCount++;
            ui->plainTextEditLog->appendPlainText(QString("Error: '%1' already exists.\n").arg(QString::fromStdString(tokens[0])));
            continue;
        }
        if (addedNames.count(tokens[0]))
        {
            errorCount++;
            ui->plainTextEditLog->appendPlainText(QString("Error: '%1' has the same name as a previously added marker.\n").arg(QString::fromStdString(tokens[0])));
            continue;
        }
        GaitSym::Body *body = nullptr;
        if (tokens[1] != "World"s)
        {
            body = m_simulation->GetBody(tokens[1]);
            if (!body && ui->checkBoxIgnoreMissingBodies->isChecked() == false)
            {
                errorCount++;
                ui->plainTextEditLog->appendPlainText(QString("Error: '%1' Body '%2' does not exist.\n").arg(QString::fromStdString(tokens[0])).arg(QString::fromStdString(tokens[1])));
                continue;
            }
        }
        std::unique_ptr<GaitSym::Marker> marker = std::make_unique<GaitSym::Marker>(nullptr);
        addedNames.insert(tokens[0]);
        marker->setSimulation(m_simulation);
        marker->setName(tokens[0]);
        marker->SetBody(body);
        marker->setSize1(Preferences::valueDouble("MarkerSize"));
        values.clear();
        for (size_t j = 2; j < tokens.size(); j++) values.push_back(GaitSym::GSUtil::Double(tokens[j]));
        for (size_t j = values.size(); j < 12; j++) values.push_back(0);
        if (ui->radioButtonPositionOnly->isChecked())
        {
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { marker->SetPosition(values[0], values[1], values[2]); }
            else { marker->SetWorldPosition(values[0], values[1], values[2]); }
        }
        else if (ui->radioButtonPositionEuler->isChecked())
        {
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { marker->SetPosition(values[0], values[1], values[2]); }
            else { marker->SetWorldPosition(values[0], values[1], values[2]); }
            if (ui->checkBoxAnglesInRadians->isChecked())
                qInput = pgd::MakeQFromEulerAnglesRadian(values[3], values[4], values[5]);
            else
                qInput = pgd::MakeQFromEulerAngles(values[3], values[4], values[5]);
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { marker->SetQuaternion(qInput.n, qInput.x, qInput.y, qInput.z); }
            else { marker->SetWorldQuaternion(qInput.n, qInput.x, qInput.y, qInput.z); }
        }
        else if (ui->radioButtonPositionAngleAxis->isChecked())
        {
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { marker->SetPosition(values[0], values[1], values[2]); }
            else { marker->SetWorldPosition(values[0], values[1], values[2]); }
            angle = values[3];
            if (!ui->checkBoxAnglesInRadians->isChecked()) angle = pgd::DegToRad(angle);
            qInput = pgd::MakeQFromAxisAngle(values[4], values[5], values[6], angle);
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { marker->SetQuaternion(qInput.n, qInput.x, qInput.y, qInput.z); }
            else { marker->SetWorldQuaternion(qInput.n, qInput.x, qInput.y, qInput.z); }
        }
        else if (ui->radioButtonPositionQuaternion->isChecked())
        {
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { marker->SetPosition(values[0], values[1], values[2]); }
            else { marker->SetWorldPosition(values[0], values[1], values[2]); }
            qInput.Set(values[3], values[4],  values[5], values[6]);
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { marker->SetQuaternion(qInput.n, qInput.x, qInput.y, qInput.z); }
            else { marker->SetWorldQuaternion(qInput.n, qInput.x, qInput.y, qInput.z); }
        }
        else if (ui->radioButtonPositionMatrix->isChecked())
        {
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { marker->SetPosition(values[0], values[1], values[2]); }
            else { marker->SetWorldPosition(values[0], values[1], values[2]); }
            qInput = pgd::MakeQfromM(pgd::Matrix3x3(values[3], values[4],  values[5],
                    values[6], values[7],  values[8],
                    values[9], values[10], values[11]));
            if (ui->checkBoxBodyLocalCoordinates->isChecked()) { marker->SetQuaternion(qInput.n, qInput.x, qInput.y, qInput.z); }
            else { marker->SetWorldQuaternion(qInput.n, qInput.x, qInput.y, qInput.z); }
        }
        ui->plainTextEditLog->appendPlainText(QString("Marker '%1' attached to '%2' created.\n").arg(QString::fromStdString(tokens[0])).arg(QString::fromStdString(tokens[1])));
        marker->saveToAttributes();
        marker->createFromAttributes();
        m_markerList->push_back(std::move(marker));
    }
    if (errorCount == 0)
    {
        ui->plainTextEditLog->appendPlainText(QString("%1 markers created. No errors detected.\n").arg(m_markerList->size()));
        QMessageBox::information(this, "Marker Import Results", QString("%1 markers created. No errors detected.").arg(m_markerList->size()), QMessageBox::Ok);
    }
    else
    {
        ui->plainTextEditLog->appendPlainText(QString("%1 markers created. %2 errors detected.\n").arg(m_markerList->size()).arg(errorCount));
        QMessageBox::information(this, "Marker Import Results", QString("%1 markers created. %2 errors detected.").arg(m_markerList->size()).arg(errorCount), QMessageBox::Ok);
    }
    return 0;
}


