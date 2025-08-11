#include "DialogStringOfPearlsBuilder.h"
#include "ui_DialogStringOfPearlsBuilder.h"

#include "MainWindow.h"
#include "Preferences.h"
#include "DialogProperties.h"

#include "Muscle.h"
#include "MAMuscle.h"
#include "MAMuscleComplete.h"
#include "DampedSpringMuscle.h"

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

    connect(ui->pushButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButtonProperties, SIGNAL(clicked()), this, SLOT(properties()));
    connect(ui->spinBoxNumberOfPearls, SIGNAL(textChanged(QString)), this, SLOT(spinBoxNumberOfPearlsChanged(QString)));

}

DialogStringOfPearlsBuilder::~DialogStringOfPearlsBuilder()
{
    delete ui;
}

void DialogStringOfPearlsBuilder::accept() // this catches OK and return/enter
{
    qDebug() << "DialogStringOfPearlsBuilder::accept()";
    Preferences::insert("DialogStringOfPearlsBuilderGeometry", saveGeometry());
    QDialog::accept();
}


void DialogStringOfPearlsBuilder::reject() // this catches cancel, close and escape key
{
    qDebug() << "DialogStringOfPearlsBuilder::reject()";
    Preferences::insert("DialogStringOfPearlsBuilderGeometry", saveGeometry());
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
    SettingsItem strapCylinderColour = Preferences::settingsItem("StrapCylinderColour");
    SettingsItem strapForceColour = Preferences::settingsItem("StrapForceColour");
    SettingsItem strapRadius = Preferences::settingsItem("StrapRadius");
    SettingsItem strapCylinderLength = Preferences::settingsItem("StrapCylinderLength");
    SettingsItem strapForceRadius = Preferences::settingsItem("StrapForceRadius");
    SettingsItem strapForceScale = Preferences::settingsItem("StrapForceScale");
    m_properties.clear();
    m_properties = { { strapColour.key, strapColour },
                    { strapCylinderColour.key, strapCylinderColour},
                    { strapForceColour.key, strapForceColour},
                    { strapRadius.key, strapRadius },
                    { strapCylinderLength.key, strapCylinderLength },
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

