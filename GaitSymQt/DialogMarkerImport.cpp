#include "DialogMarkerImport.h"
#include "ui_DialogMarkerImport.h"

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

DialogMarkerImport::DialogMarkerImport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMarkerImport)
{
    ui->setupUi(this);

    setWindowTitle(tr("Marker Import/Export"));
#ifdef Q_OS_MACOS
    setWindowFlags(windowFlags() & (~Qt::Dialog) |
                   Qt::Window); // allows the window to be resized on macs
#endif

    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &DialogMarkerImport::reject);
    connect(ui->pushButtonApply, &QAbstractButton::clicked, this, &DialogMarkerImport::applyButtonClicked);
    connect(ui->pushButtonProcess, &QAbstractButton::clicked, this, &DialogMarkerImport::processButtonClicked);
    connect(ui->lineEditFileName, &QLineEdit::textChanged, this, &DialogMarkerImport::fileNameTextChanged);

    ui->lineEditFileName->setPathType(LineEditPath::FileForOpen);

    SetUIElementsFromPreferences();

    EnableWidgets();

    restoreGeometry(Preferences::valueQByteArray("DialogMarkerImportGeometry"));
}

DialogMarkerImport::~DialogMarkerImport()
{
    delete ui;
}

void DialogMarkerImport::processButtonClicked() // this catches OK and return/enter
{
    qDebug() << "DialogMarkerImport::applyButtonClicked()";
    ImportMarkers();
    SaveUIElementsToPreferences();
    Preferences::insert("DialogMarkerImportGeometry", saveGeometry());
}

void DialogMarkerImport::applyButtonClicked() // this catches cancel, close and escape key
{
    qDebug() << "DialogMarkerImport::closeButtonClicked()";
    SaveUIElementsToPreferences();
    Preferences::insert("DialogMarkerImportGeometry", saveGeometry());
    QDialog::accept();
}

void DialogMarkerImport::reject() // this catches cancel, close and escape key
{
    qDebug() << "DialogGeoms::reject()";
    Preferences::insert("DialogMarkerImportGeometry", saveGeometry());
    QDialog::reject();
}

void DialogMarkerImport::closeEvent(QCloseEvent *event)
{
    qDebug() << "DialogMarkerImportGeometry::closeEvent()";
    SaveUIElementsToPreferences();
    Preferences::insert("DialogMarkerImportGeometry", saveGeometry());
    QDialog::closeEvent(event);
}

std::vector<std::unique_ptr<GaitSym::Marker> > *DialogMarkerImport::markerList() const
{
    return m_markerList;
}

void DialogMarkerImport::setMarkerList(std::vector<std::unique_ptr<GaitSym::Marker> > *markerList)
{
    m_markerList = markerList;
}

void DialogMarkerImport::setSimulation(GaitSym::Simulation *simulation)
{
    m_simulation = simulation;
}

void DialogMarkerImport::SetUIElementsFromPreferences()
{
    int formatOption = Preferences::valueInt("DialogMarkerImportFormatOption");
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
    int separatorOption = Preferences::valueInt("DialogMarkerImportSeparatorOption");
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
    ui->checkBoxHeaderRow->setChecked(Preferences::valueBool("DialogMarkerImportHeaderRow"));
    ui->checkBoxQuotedStrings->setChecked(Preferences::valueBool("DialogMarkerImportQuotedStrings"));
    ui->checkBoxAnglesInRadians->setChecked(Preferences::valueBool("DialogMarkerImportAnglesInRadians"));
    ui->checkBoxAllowOverwrite->setChecked(Preferences::valueBool("DialogMarkerImportAllowOverwrite"));
    ui->checkBoxIgnoreMissingBodies->setChecked(Preferences::valueBool("DialogMarkerIgnoreMissingBodies"));
    ui->checkBoxBodyLocalCoordinates->setChecked(Preferences::valueBool("DialogMarkerIgnoreBodyLocalCoordinates"));

    ui->lineEditFileName->setText(Preferences::valueQString("DialogMarkerImportFileName"));
}

void DialogMarkerImport::SaveUIElementsToPreferences()
{
    int formatOption = 0;
    if (ui->radioButtonPositionOnly->isChecked()) formatOption = 0;
    if (ui->radioButtonPositionEuler->isChecked()) formatOption = 1;
    if (ui->radioButtonPositionAngleAxis->isChecked()) formatOption = 2;
    if (ui->radioButtonPositionQuaternion->isChecked()) formatOption = 3;
    if (ui->radioButtonPositionMatrix->isChecked()) formatOption = 4;
    Preferences::insert("DialogMarkerImportFormatOption", formatOption);

    int separatorOption = 0;
    if (ui->radioButtonCommaSeparator->isChecked()) separatorOption = 0;
    if (ui->radioButtonTabSeparator->isChecked()) separatorOption = 1;
    if (ui->radioButtonWhiteSpaceSeparator->isChecked()) separatorOption = 2;
    Preferences::insert("DialogMarkerImportSeparatorOption", separatorOption);

    Preferences::insert("DialogMarkerImportHeaderRow", ui->checkBoxHeaderRow->isChecked());
    Preferences::insert("DialogMarkerImportQuotedStrings", ui->checkBoxQuotedStrings->isChecked());
    Preferences::insert("DialogMarkerImportAnglesInRadians", ui->checkBoxAnglesInRadians->isChecked());
    Preferences::insert("DialogMarkerImportAllowOverwrite", ui->checkBoxAllowOverwrite->isChecked());
    Preferences::insert("DialogMarkerIgnoreMissingBodies", ui->checkBoxIgnoreMissingBodies->isChecked());
    Preferences::insert("DialogMarkerIgnoreBodyLocalCoordinates", ui->checkBoxBodyLocalCoordinates->isChecked());

    Preferences::insert("DialogMarkerImportFileName", ui->lineEditFileName->text());
}

int DialogMarkerImport::ImportMarkers()
{
    Q_ASSERT_X(m_markerList, "DialogMarkerImport::ImportMarkers", "m_markerList not defined");
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
        if (m_addedNames.count(tokens[0]))
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
        m_addedNames.insert(tokens[0]);
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
        ui->plainTextEditLog->appendPlainText(QString("%1 will be markers created. No errors detected.\n").arg(m_markerList->size()));
        // QMessageBox::information(this, "Marker Import Results", QString("%1 markers will be created. No errors detected.").arg(m_markerList->size()), QMessageBox::Ok);
    }
    else
    {
        ui->plainTextEditLog->appendPlainText(QString("%1 markers will be created. %2 errors detected.\n").arg(m_markerList->size()).arg(errorCount));
        // QMessageBox::information(this, "Marker Import Results", QString("%1 markers will be created. %2 errors detected.").arg(m_markerList->size()).arg(errorCount), QMessageBox::Ok);
    }
    if (m_markerList->size()) { ui->pushButtonApply->setEnabled(true); }
    return 0;
}

void DialogMarkerImport::EnableWidgets()
{
    bool fileAvailable = QFileInfo(ui->lineEditFileName->text()).isFile();
    ui->pushButtonProcess->setEnabled(fileAvailable);
    ui->pushButtonApply->setEnabled(m_markerList && m_markerList->size() > 0);
}

void DialogMarkerImport::fileNameTextChanged(const QString &text)
{
    EnableWidgets();
}
#
