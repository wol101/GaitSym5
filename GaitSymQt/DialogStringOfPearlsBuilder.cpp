#include "DialogStringOfPearlsBuilder.h"
#include "ui_DialogStringOfPearlsBuilder.h"

#include "MainWindow.h"
#include "Preferences.h"
#include "DialogProperties.h"

#include "Muscle.h"
#include "MAMuscle.h"
#include "MAMuscleComplete.h"
#include "DampedSpringMuscle.h"
#include "TwoPointStrap.h"
#include "NPointStrap.h"
#include "CylinderWrapStrap.h"
#include "TwoCylinderWrapStrap.h"
#include "GSUtil.h"
#include "SphereGeom.h"

using namespace std::string_literals; // enables s-suffix for std::string literals

DialogStringOfPearlsBuilder::DialogStringOfPearlsBuilder(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogStringOfPearlsBuilder)
{
    ui->setupUi(this);
    MainWindow::layoutSpacing(this);

    setWindowTitle(tr("Muscle Builder"));
#ifdef Q_OS_MACOS
    setWindowFlags(windowFlags() & (~Qt::Dialog) | Qt::Window); // allows the window to be resized on macs
#endif
    restoreGeometry(Preferences::valueQByteArray("DialogStringOfPearlsBuilderGeometry"));
    ui->splitter->restoreState(Preferences::valueQByteArray("DialogStringOfPearlsBuilderSplitterState"));

    connect(ui->pushButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButtonProperties, SIGNAL(clicked()), this, SLOT(properties()));
    connect(ui->pushButtonImportPathFromMuscle, SIGNAL(clicked()), this, SLOT(importPathFromMuscle()));
    connect(ui->spinBoxNumberOfPearls, SIGNAL(textChanged(QString)), this, SLOT(spinBoxNumberOfPearlsChanged(QString)));

}

DialogStringOfPearlsBuilder::~DialogStringOfPearlsBuilder()
{
    delete ui;
}

void DialogStringOfPearlsBuilder::accept() // this catches OK and return/enter
{
    createBodies();
    createGeoms();
    createMuscles();

    qDebug() << "DialogStringOfPearlsBuilder::accept()";
    Preferences::insert("DialogStringOfPearlsBuilderGeometry", saveGeometry());
    Preferences::insert("DialogStringOfPearlsBuilderSplitterState", ui->splitter->saveState());
    QDialog::accept();
}


void DialogStringOfPearlsBuilder::reject() // this catches cancel, close and escape key
{
    qDebug() << "DialogStringOfPearlsBuilder::reject()";
    Preferences::insert("DialogStringOfPearlsBuilderGeometry", saveGeometry());
    Preferences::insert("DialogStringOfPearlsBuilderSplitterState", ui->splitter->saveState());
    QDialog::reject();
}

// void DialogStringOfPearlsBuilder::closeEvent(QCloseEvent *event)
// {
//     qDebug() << "DialogMuscles::closeEvent()";
//     Preferences::insert("DialogStringOfPearlsBuilderGeometry", saveGeometry());
//     QDialog::closeEvent(event);
// }

void DialogStringOfPearlsBuilder::properties()
{
    DialogProperties dialogProperties(this);

    SettingsItem strapColour = Preferences::settingsItem("StrapColour");
    SettingsItem strapForceColour = Preferences::settingsItem("StrapForceColour");
    SettingsItem strapRadius = Preferences::settingsItem("StrapRadius");
    SettingsItem strapForceRadius = Preferences::settingsItem("StrapForceRadius");
    SettingsItem strapForceScale = Preferences::settingsItem("StrapForceScale");
    m_properties.clear();
    m_properties = { { strapColour.key, strapColour },
                    { strapForceColour.key, strapForceColour},
                    { strapRadius.key, strapRadius },
                    { strapForceRadius.key, strapForceRadius },
                    { strapForceScale.key, strapForceScale } };
    dialogProperties.setInputSettingsItems(m_properties);
    dialogProperties.initialise();

    int status = dialogProperties.exec();
    if (status == QDialog::Accepted)
    {
        dialogProperties.update();
        m_properties = dialogProperties.getOutputSettingsItems();
    }
}

void DialogStringOfPearlsBuilder::setSimulation(GaitSym::Simulation *newSimulation)
{
    m_simulation = newSimulation;
}

void DialogStringOfPearlsBuilder::updateActivation()
{
    bool okEnable = true;
    ui->pushButtonOK->setEnabled(okEnable);
}

void DialogStringOfPearlsBuilder::lateInitialise()
{
    Q_ASSERT_X(m_simulation, "DialogStringOfPearlsBuilder::lateInitialise", "m_simulation undefined");

    // fill the muscle combobox
    auto muscleList = m_simulation->GetMuscleList();
    QStringList muscleIDs;
    for (auto && it : *muscleList) { muscleIDs.append(QString::fromStdString(it.first)); }
    ui->comboBoxImportFromMuscle->addItems(muscleIDs);

    // fill the marker comboboxes
    auto markerList = m_simulation->GetMarkerList();
    QStringList markerIDs;
    for (auto && it : *markerList) { markerIDs.append(QString::fromStdString(it.first)); }
    ui->comboBoxOriginMarkerID->addItems(markerIDs);
    ui->comboBoxInsertionMarkerID->addItems(markerIDs);

    // now set some sensible defaults
    ui->lineEditForcePerUnitArea->setBottom(0);
    ui->lineEditForcePerUnitArea->setValue(300000);
    ui->lineEditVMaxFactor->setBottom(0);
    ui->lineEditVMaxFactor->setValue(8.4);
    ui->lineEditPCA->setBottom(0);
    ui->lineEditPCA->setValue(1);
    ui->lineEditFibreLength->setBottom(0);
    ui->lineEditFibreLength->setValue(1);
    ui->lineEditActivationK->setBottom(0);
    ui->lineEditActivationK->setValue(0.17);

    ui->lineEditForcePerUnitArea_2->setBottom(0);
    ui->lineEditForcePerUnitArea_2->setValue(300000);
    ui->lineEditVMaxFactor_2->setBottom(0);
    ui->lineEditVMaxFactor_2->setValue(8.4);
    ui->lineEditPCA_2->setBottom(0);
    ui->lineEditPCA_2->setValue(1);
    ui->lineEditFibreLength_2->setBottom(0);
    ui->lineEditFibreLength_2->setValue(1);
    ui->lineEditActivationK_2->setBottom(0);
    ui->lineEditActivationK_2->setValue(0.17);
    ui->lineEditWidth->setBottom(0);
    ui->lineEditWidth->setValue(0.5);
    ui->lineEditTendonLength->setBottom(0);
    ui->lineEditTendonLength->setValue(1);
    ui->lineEditSerialStrainAtFMax->setBottom(0);
    ui->lineEditSerialStrainAtFMax->setValue(0.06);
    ui->lineEditSerialStrainRateAtFMax->setBottom(0);
    ui->lineEditSerialStrainRateAtFMax->setValue(0);
    ui->lineEditParallelStrainAtFMax->setBottom(0);
    ui->lineEditParallelStrainAtFMax->setValue(0.6);
    ui->lineEditParallelStrainRateAtFMax->setBottom(0);
    ui->lineEditParallelStrainRateAtFMax->setValue(0);
    ui->lineEditInitialFibreLength->setBottom(0);
    ui->lineEditInitialFibreLength->setValue(1);
    ui->checkBoxUseActivation->setChecked(false);
    ui->lineEditFastTwitchProportion->setBottom(0);
    ui->lineEditFastTwitchProportion->setValue(0.5);
    ui->lineEditTActivationA->setBottom(0);
    ui->lineEditTActivationA->setValue(8.00E-02);
    ui->lineEditTActivationB->setBottom(0);
    ui->lineEditTActivationB->setValue(4.70E-04);
    ui->lineEditTDeactivationA->setBottom(0);
    ui->lineEditTDeactivationA->setValue(9.00E-02);
    ui->lineEditTDeactivationB->setBottom(0);
    ui->lineEditTDeactivationB->setValue(5.60E-04);
    ui->lineEditActivationRate->setBottom(0);
    ui->lineEditActivationRate->setValue(500);
    ui->lineEditInitialActivation->setBottom(0);
    ui->lineEditInitialActivation->setValue(0);
    ui->lineEditMinimumActivation->setBottom(0);
    ui->lineEditMinimumActivation->setValue(1e-5);

    ui->lineEditUnloadedLength->setBottom(0);
    ui->lineEditUnloadedLength->setValue(1);
    ui->lineEditSpringConstant->setBottom(0);
    ui->lineEditSpringConstant->setValue(1);
    ui->lineEditArea->setBottom(0);
    ui->lineEditArea->setValue(1);
    ui->lineEditDamping->setBottom(0);
    ui->lineEditDamping->setValue(1);

    ui->lineEditPearlRadius->setValue(1);
    ui->lineEditPearlMass->setValue(1);
    ui->spinBoxNumberOfPearls->setValue(1);

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(3);
    QStringList columnHeadings;
    columnHeadings << "X" << "Y" << "Z";
    ui->tableWidget->setHorizontalHeaderLabels(columnHeadings);
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
        {
            ui->tableWidget->setCellWidget(row, col, new LineEditDouble());
        }
    }
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
    {
        ui->tableWidget->setColumnWidth(col, ui->tableWidget->columnWidth(col) * 2);
    }


    // set default new name
    auto nameSet = m_simulation->GetNameSet();
    ui->lineEditRootID->addStrings(nameSet);
    int initialNameCount = 0;
    QString initialName = QString("StringOfPearls%1").arg(initialNameCount, 3, 10, QLatin1Char('0'));
    while (true)
    {
        bool nameAcceptable = true;
        for (auto &&name : nameSet)
        {
            if (QString::fromStdString(name).startsWith(initialName))
            {
                nameAcceptable = false;
                break;
            }
        }
        if (nameAcceptable)
        {
            break;
        }
        else
        {
            initialNameCount++;
            initialName = QString("Muscle%1").arg(initialNameCount, 3, 10, QLatin1Char('0'));
            if (initialNameCount >= 999) break; // only do this for the first 999 markers
        }
    }
    ui->lineEditRootID->setText(initialName);
}

void DialogStringOfPearlsBuilder::spinBoxNumberOfPearlsChanged(const QString &text)
{
    (void)text; // stops warnings about argument being unused
    int rows = ui->tableWidget->rowCount();
    int newRows = ui->spinBoxNumberOfPearls->value() + 2;
    Q_ASSERT_X(newRows >= 3, "DialogStringOfPearlsBuilder::spinBoxNumberOfPearlsChanged", "newRows too low");
    if (newRows < rows)
    {
        for (int rowToDelete = rows - 1; rowToDelete >= newRows; --rowToDelete)
        {
            ui->tableWidget->removeRow(rowToDelete);
        }
    }
    if (newRows > rows)
    {
        for (int rowToAdd = rows; rowToAdd < newRows; ++rowToAdd)
        {
            ui->tableWidget->insertRow(rowToAdd);
            for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
            {
                ui->tableWidget->setCellWidget(rowToAdd, col, new LineEditDouble());
            }
        }
    }
}

std::vector<std::unique_ptr<GaitSym::Strap> > *DialogStringOfPearlsBuilder::strapList()
{
    return &m_strapList;
}

std::vector<std::unique_ptr<GaitSym::Muscle> > *DialogStringOfPearlsBuilder::muscleList()
{
    return &m_muscleList;
}

std::vector<std::unique_ptr<GaitSym::Geom> > *DialogStringOfPearlsBuilder::geomList()
{
    return &m_geomList;
}

std::vector<std::unique_ptr<GaitSym::Marker> > *DialogStringOfPearlsBuilder::markerList()
{
    return &m_markerList;
}

std::vector<std::unique_ptr<GaitSym::Body> > *DialogStringOfPearlsBuilder::bodyList()
{
    return &m_bodyList;
}

void DialogStringOfPearlsBuilder::importPathFromMuscle()
{
    std::string muscleID = ui->comboBoxImportFromMuscle->currentText().toStdString();
    GaitSym::Muscle *muscle = m_simulation->GetMuscle(muscleID);
    if (!muscle) return;
    GaitSym::Strap *strap = muscle->GetStrap();

    std::vector<pgd::Vector3> pathCoordinates;
    std::string originID;
    std::string insertionID;
    while (true)
    {
        if (GaitSym::TwoPointStrap *twoPointStrap = dynamic_cast<GaitSym::TwoPointStrap *>(strap))
        {
            std::vector<std::unique_ptr<GaitSym::PointForce >> *pointForceList = twoPointStrap->GetPointForceList();
            pathCoordinates.reserve(pointForceList->size());
            pathCoordinates.push_back(pgd::Vector3(pointForceList->at(0)->point[0], pointForceList->at(0)->point[1], pointForceList->at(0)->point[2]));
            pathCoordinates.push_back(pgd::Vector3(pointForceList->at(1)->point[0], pointForceList->at(1)->point[1], pointForceList->at(1)->point[2]));
            originID = twoPointStrap->GetOriginMarker()->name();
            insertionID = twoPointStrap->GetInsertionMarker()->name();
            break;
        }

        if (GaitSym::NPointStrap *nPointStrap = dynamic_cast<GaitSym::NPointStrap *>(strap))
        {
            std::vector<std::unique_ptr<GaitSym::PointForce >> *pointForceList = nPointStrap->GetPointForceList();
            pathCoordinates.reserve(pointForceList->size());
            pathCoordinates.push_back(pgd::Vector3(pointForceList->at(0)->point[0], pointForceList->at(0)->point[1], pointForceList->at(0)->point[2]));
            for (size_t i = 2; i < pointForceList->size(); i++) pathCoordinates.push_back(pgd::Vector3(pointForceList->at(i)->point[0], pointForceList->at(i)->point[1], pointForceList->at(i)->point[2]));
            pathCoordinates.push_back(pgd::Vector3(pointForceList->at(1)->point[0], pointForceList->at(1)->point[1], pointForceList->at(1)->point[2]));
            originID = nPointStrap->GetOriginMarker()->name();
            insertionID = nPointStrap->GetInsertionMarker()->name();
            break;
        }

        if (GaitSym::CylinderWrapStrap *cylinderWrapStrap = dynamic_cast<GaitSym::CylinderWrapStrap *>(strap))
        {
            pathCoordinates = *cylinderWrapStrap->GetPathCoordinates();
            originID = cylinderWrapStrap->GetOriginMarker()->name();
            insertionID = cylinderWrapStrap->GetInsertionMarker()->name();
            break;
        }

        if (GaitSym::TwoCylinderWrapStrap *twoCylinderWrapStrap = dynamic_cast<GaitSym::TwoCylinderWrapStrap *>(strap))
        {
            pathCoordinates = *twoCylinderWrapStrap->GetPathCoordinates();
            originID = twoCylinderWrapStrap->GetOriginMarker()->name();
            insertionID = twoCylinderWrapStrap->GetInsertionMarker()->name();
            break;
        }

        qDebug() << "Error in DialogStringOfPearlsBuilder::spinBoxNumberOfPearlsChanged: Unsupported STRAP type";
        return;
    }

    int index = ui->comboBoxOriginMarkerID->findText(QString::fromStdString(originID));
    ui->comboBoxOriginMarkerID->setCurrentIndex(index);
    index = ui->comboBoxInsertionMarkerID->findText(QString::fromStdString(insertionID));
    ui->comboBoxInsertionMarkerID->setCurrentIndex(index);

    std::vector<pgd::Vector3> pathVectors(pathCoordinates.size() - 1);
    std::vector<double> pathVectorsLength(pathCoordinates.size() - 1);
    std::vector<double> pathVectorsCumulativeLength(pathCoordinates.size());
    double totalLength = 0;
    for (size_t i = 0; i < pathVectors.size(); ++i)
    {
        pathVectors[i] = pathCoordinates[i + 1] - pathCoordinates[i];
        pathVectorsLength[i] = pathVectors[i].Magnitude();
        totalLength += pathVectorsLength[i];
        pathVectorsCumulativeLength[i] = totalLength;
    }
    int numPearls = ui->spinBoxNumberOfPearls->value();
    std::vector<pgd::Vector3> stringOfPearlsPath((size_t)numPearls + 2);
    double lengthIncrement = totalLength / (numPearls + 1);
    stringOfPearlsPath.front() = pathCoordinates.front();
    stringOfPearlsPath.back() = pathCoordinates.back();
    double lengthTarget = 0;
    for (size_t i = 1; i < numPearls + 1; ++i)
    {
        lengthTarget += lengthIncrement;
        // lower_bound
        // if a searching element exists: std::lower_bound() returns iterator to the element itself
        // if a searching element doesn't exist:
        //    if all elements are greater than the searching element: lower_bound() returns an iterator to begin of the range
        //    if all elements are lower than the searching element: lower_bound() returns an iterator to end of the range
        //    otherwise, lower_bound() returns an iterator to the next greater element to the search element of the range
        auto it = std::lower_bound(pathVectorsCumulativeLength.begin(), pathVectorsCumulativeLength.end(), lengthTarget);
        if (it == pathVectorsCumulativeLength.end()) { qDebug() << "Error in DialogStringOfPearlsBuilder::spinBoxNumberOfPearlsChanged: lengthTarget out of range"; return; }
        std::size_t index = std::distance(std::begin(pathVectorsCumulativeLength), it);
        if (*it == lengthTarget)
        {
            stringOfPearlsPath[i] = pathCoordinates[index + 1];
        }
        else
        {
            double deltaLength;
            index > 0 ? deltaLength = lengthTarget - pathVectorsCumulativeLength[index - 1] : deltaLength = lengthTarget;
            stringOfPearlsPath[i] = pathCoordinates[index] + pathVectors[index] * (deltaLength / pathVectorsLength[index]);
        }
    }

    for (size_t i = 0; i < stringOfPearlsPath.size(); ++i)
    {
        pgd::Vector3 v = stringOfPearlsPath[i];
        dynamic_cast<LineEditDouble *>(ui->tableWidget->cellWidget(int(i), 0))->setValue(v.x);
        dynamic_cast<LineEditDouble *>(ui->tableWidget->cellWidget(int(i), 1))->setValue(v.y);
        dynamic_cast<LineEditDouble *>(ui->tableWidget->cellWidget(int(i), 2))->setValue(v.z);
    }
}

void DialogStringOfPearlsBuilder::createBodies()
{
    double mass = ui->lineEditPearlMass->value();
    double radius = ui->lineEditPearlRadius->value();
    double moi = (2.0/5.0)*mass*radius*radius; // moment of interia of a solid sphere
    int numPearls = ui->spinBoxNumberOfPearls->value();
    std::string rootID = ui->lineEditRootID->text().toStdString();
    for (size_t i = 0; i < numPearls; i++)
    {
        pgd::Vector3 position;
        int row = int(i + 1);
        position.x = dynamic_cast<LineEditDouble *>(ui->tableWidget->cellWidget(row, 0))->value();
        position.y = dynamic_cast<LineEditDouble *>(ui->tableWidget->cellWidget(row, 1))->value();
        position.z = dynamic_cast<LineEditDouble *>(ui->tableWidget->cellWidget(row, 2))->value();
        auto body = std::make_unique<GaitSym::Body>();
        body->SetConstructionPosition(position);
        body->SetPosition(position);
        body->SetMass(mass, moi, moi, moi, 0, 0, 0);
        std::string bodyID = GaitSym::GSUtil::ToString("%s_body_%03zu", rootID.c_str(), i);
        body->setName(bodyID);
        body->setSimulation(m_simulation);
        body->EnterConstructionMode();
        m_bodyList.push_back(std::move(body));
    }
}

void DialogStringOfPearlsBuilder::createGeoms()
{
    double radius = ui->lineEditPearlRadius->value();
    int numPearls = ui->spinBoxNumberOfPearls->value();
    std::string rootID = ui->lineEditRootID->text().toStdString();
    for (size_t i = 0; i < numPearls; ++i)
    {
        auto marker = std::make_unique<GaitSym::Marker>(m_bodyList[i].get());
        std::string markerID = GaitSym::GSUtil::ToString("%s_geom_marker_%03zu", rootID.c_str(), i);
        marker->setName(markerID);
        marker->setSimulation(m_simulation);
        marker->setUpstreamObjects( { m_bodyList[i].get() } );

        auto sphereGeom = std::make_unique<GaitSym::SphereGeom>(radius);
        std::string geomID = GaitSym::GSUtil::ToString("%s_geom_%03zu", rootID.c_str(), i);
        sphereGeom->setName(geomID);
        sphereGeom->setGeomMarker(marker.get());
        sphereGeom->setSimulation(m_simulation);
        sphereGeom->setUpstreamObjects( { marker.get() } );

        m_markerList.push_back(std::move(marker));
        m_geomList.push_back(std::move(sphereGeom));
    }
}

void DialogStringOfPearlsBuilder::createMuscles()
{
    std::string rootID = ui->lineEditRootID->text().toStdString();
    int numPearls = ui->spinBoxNumberOfPearls->value();
    GaitSym::Marker *originMarker = m_simulation->GetMarker(ui->comboBoxOriginMarkerID->currentText().toStdString());
    GaitSym::Marker *insertionMarker = nullptr;
    std::unique_ptr<GaitSym::Muscle> outputMuscle;
    for (size_t i = 0; i < numPearls; ++i)
    {
        insertionMarker = m_markerList[i].get();
        auto strap = std::make_unique<GaitSym::TwoPointStrap>();
        strap->SetOrigin(originMarker);
        strap->SetInsertion(insertionMarker);


        QString muscleTab = ui->tabWidgetMuscle->tabText(ui->tabWidgetMuscle->currentIndex());
        if (muscleTab == "Minetti-Alexander")
        {
            std::unique_ptr<GaitSym::MAMuscle> muscle = std::make_unique<GaitSym::MAMuscle>();
            muscle->SetStrap(strap.get());
            double forcePerUnitArea = ui->lineEditForcePerUnitArea->value();
            double vMaxFactor = ui->lineEditVMaxFactor->value();
            double pca = ui->lineEditPCA->value();
            double fibreLength = ui->lineEditFibreLength->value();
            double activationK = ui->lineEditActivationK->value();
            muscle->setForcePerUnitArea(forcePerUnitArea);
            muscle->setVMaxFactor(vMaxFactor);
            muscle->setPca(pca);
            muscle->setFibreLength(fibreLength);
            muscle->SetF0(pca * forcePerUnitArea);
            muscle->SetVMax(fibreLength * vMaxFactor);
            muscle->SetK(activationK);
            outputMuscle = std::move(muscle);
        }
        else if (muscleTab == "Minetti-Alexander Elastic")
        {
            std::unique_ptr<GaitSym::MAMuscleComplete> muscle = std::make_unique<GaitSym::MAMuscleComplete>();
            muscle->SetStrap(strap.get());
            double forcePerUnitArea = ui->lineEditForcePerUnitArea_2->value();
            double vMaxFactor = ui->lineEditVMaxFactor_2->value();
            double pca = ui->lineEditPCA_2->value();
            double fibreLength = ui->lineEditFibreLength_2->value();
            double activationK = ui->lineEditActivationK_2->value();
            double width = ui->lineEditWidth->value();
            double tendonLength = ui->lineEditTendonLength->value();
            double serialStrainAtFmax = ui->lineEditSerialStrainAtFMax->value();
            double serialStrainRateAtFmax = ui->lineEditSerialStrainRateAtFMax->value();
            QString serialStrainModel = ui->comboBoxSerialStrainModel->currentText();
            double parallelStrainAtFmax = ui->lineEditParallelStrainAtFMax->value();
            double parallelStrainRateAtFmax = ui->lineEditParallelStrainRateAtFMax->value();
            QString parallelStrainModel = ui->comboBoxParallelStrainModel->currentText();
            double parallelElementLength = fibreLength;
            double vMax = fibreLength * vMaxFactor;
            double fMax = pca * forcePerUnitArea;
            double initialFibreLength = ui->lineEditInitialFibreLength->value();
            bool activationKinetics = ui->checkBoxUseActivation->isChecked();
            double akFastTwitchProportion = ui->lineEditFastTwitchProportion->value();
            double akTActivationA = ui->lineEditTActivationA->value();
            double akTActivationB = ui->lineEditTActivationB->value();
            double akTDeactivationA = ui->lineEditTDeactivationA->value();
            double akTDeactivationB = ui->lineEditTDeactivationB->value();
            double activationRate = ui->lineEditActivationRate->value();
            double startActivation = ui->lineEditInitialActivation->value();
            double minimumActivation = ui->lineEditMinimumActivation->value();
            muscle->setSerialStrainModel(serialStrainModel.toStdString());
            muscle->setParallelStrainModel(parallelStrainModel.toStdString());
            muscle->setActivationK(activationK);
            muscle->setAkFastTwitchProportion(akFastTwitchProportion);
            muscle->setAkTActivationA(akTActivationA);
            muscle->setAkTActivationB(akTActivationB);
            muscle->setAkTDeactivationA(akTDeactivationA);
            muscle->setAkTDeactivationB(akTDeactivationB);
            muscle->setFibreLength(fibreLength);
            muscle->setForcePerUnitArea(forcePerUnitArea);
            muscle->setInitialFibreLength(initialFibreLength);
            muscle->setPca(pca);
            muscle->setStartActivation(startActivation);
            muscle->setTendonLength(tendonLength);
            muscle->setVMaxFactor(vMaxFactor);
            muscle->setWidth(width);

            muscle->SetSerialElasticProperties(serialStrainAtFmax, serialStrainRateAtFmax, tendonLength, muscle->serialStrainModel());
            muscle->SetParallelElasticProperties(parallelStrainAtFmax, parallelStrainRateAtFmax, parallelElementLength, muscle->parallelStrainModel());
            muscle->SetMuscleProperties(vMax, fMax, activationK, width);
            muscle->SetActivationKinetics(activationKinetics, akFastTwitchProportion, akTActivationA, akTActivationB, akTDeactivationA, akTDeactivationB);
            muscle->SetInitialFibreLength(initialFibreLength);
            muscle->SetActivationRate(activationRate);
            muscle->SetStartActivation(startActivation);
            muscle->SetMinimumActivation(minimumActivation);
            outputMuscle = std::move(muscle);
        }
        else if (muscleTab == "Damped Spring")
        {
            std::unique_ptr<GaitSym::DampedSpringMuscle> muscle = std::make_unique<GaitSym::DampedSpringMuscle>();
            muscle->SetStrap(strap.get());
            double unloadedLength = ui->lineEditUnloadedLength->value();
            double springConstant = ui->lineEditSpringConstant->value();
            double area = ui->lineEditArea->value();
            double damping = ui->lineEditDamping->value();
            muscle->SetUnloadedLength(unloadedLength);
            muscle->SetSpringConstant(springConstant);
            muscle->SetArea(area);
            muscle->SetDamping(damping);
            if (ui->lineEditBreakingStrain->text().size()) muscle->SetBreakingStrain(ui->lineEditBreakingStrain->value());
            outputMuscle = std::move(muscle);
        }
        Q_ASSERT_X(outputMuscle, "DialogStringOfPearlsBuilder::createMuscles", "outputMuscle undefined");
        std::string muscleID = GaitSym::GSUtil::ToString("%s_link_%03zu", rootID.c_str(), i);
        outputMuscle->setName(muscleID);
        outputMuscle->setSimulation(m_simulation);

        m_strapList.push_back(std::move(strap));
        originMarker = insertionMarker;
    }


}


