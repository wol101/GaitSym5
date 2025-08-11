#include "DialogStringOfPearlsBuilder.h"
#include "ui_DialogStringOfPearlsBuilder.h"

#include "MainWindow.h"
#include "Preferences.h"
#include "DialogProperties.h"

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

void DialogStringOfPearlsBuilder::updateActivation()
{
    bool okEnable = true;
    ui->pushButtonOK->setEnabled(okEnable);
}
