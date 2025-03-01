#include "DialogBodyBuilder.h"
#include "ui_DialogBodyBuilder.h"

#include "SimulationWidget.h"
#include "Simulation.h"
#include "Preferences.h"
#include "FacetedObject.h"
#include "Body.h"
#include "LineEditUniqueName.h"
#include "DialogProperties.h"
#include "MainWindow.h"

#include "pystring.h"

#include <QBoxLayout>
#include <QFileInfo>
#include <QProgressDialog>
#include <QThread>
#include <QValidator>
#include <QMessageBox>

DialogBodyBuilder::DialogBodyBuilder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBodyBuilder)
{
    ui->setupUi(this);
    MainWindow::layoutSpacing(this);

    setWindowTitle(tr("Body Builder"));
#ifdef Q_OS_MACOS
    setWindowFlags(windowFlags() & (~Qt::Dialog) | Qt::Window); // allows the window to be resized on macs
#endif
    restoreGeometry(Preferences::valueQByteArray("DialogBodyBuilderGeometry"));
    ui->checkBoxMoveMarkers->setChecked(Preferences::valueBool("DialogBodyBuilderMoveMarkers", false));

    connect(ui->pushButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButtonProperties, SIGNAL(clicked()), this, SLOT(properties()));
    connect(ui->pushButtonCalculate, SIGNAL(clicked()), this, SLOT(calculate()));
    connect(ui->lineEditMesh1, SIGNAL(focussed(bool)), this, SLOT(lineEditMeshClicked()));
    connect(ui->lineEditMesh2, SIGNAL(focussed(bool)), this, SLOT(lineEditMeshClicked()));
    connect(ui->lineEditMesh3, SIGNAL(focussed(bool)), this, SLOT(lineEditMeshClicked()));
    connect(ui->lineEditMesh1, SIGNAL(editingFinished()), this, SLOT(lineEditMeshClicked()));
    connect(ui->lineEditMesh2, SIGNAL(editingFinished()), this, SLOT(lineEditMeshClicked()));
    connect(ui->lineEditMesh3, SIGNAL(editingFinished()), this, SLOT(lineEditMeshClicked()));
    connect(ui->lineEditID, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditIDTextChanged(const QString &)));

    ui->pushButtonCalculate->setEnabled(false);
    ui->pushButtonOK->setEnabled(false);

    ui->lineEditDensity->setValue(1.0);
    ui->lineEditMass->setValue(1.0);
    ui->lineEditI11->setValue(1.0);
    ui->lineEditI22->setValue(1.0);
    ui->lineEditI33->setValue(1.0);
    ui->lineEditDensity->setBottom(std::numeric_limits<double>::min());
    ui->lineEditMass->setBottom(std::numeric_limits<double>::min());
    ui->lineEditI11->setBottom(std::numeric_limits<double>::min());
    ui->lineEditI22->setBottom(std::numeric_limits<double>::min());
    ui->lineEditI33->setBottom(std::numeric_limits<double>::min());

    qobject_cast<LineEditPath *>(ui->lineEditMesh1)->setPathType(LineEditPath::FileForOpen);
    qobject_cast<LineEditPath *>(ui->lineEditMesh2)->setPathType(LineEditPath::FileForOpen);
    qobject_cast<LineEditPath *>(ui->lineEditMesh3)->setPathType(LineEditPath::FileForOpen);

    setInputBody(nullptr);
}

DialogBodyBuilder::~DialogBodyBuilder()
{
    delete ui;
}

void DialogBodyBuilder::lateInitialise()
{
    Q_ASSERT_X(m_simulation, "DialogBodyBuilder::lateInitialise", "m_simulation not defined");

    GaitSym::Body referenceBody;
    referenceBody.SetConstructionDensity(Preferences::valueDouble("BodyDensity", 1000.0));
    if (m_inputBody)
    {
        Q_ASSERT_X(m_inputBody, "DialogBodyBuilder::lateInitialise", "m_simulation not defined");
        ui->lineEditID->setText(QString::fromStdString(m_inputBody->name()));
        ui->lineEditID->setEnabled(false);
    }
    else
    {
        QString name("World"); // World always exists
        ui->lineEditID->addString(name);
        auto nameSet = m_simulation->GetNameSet();
        ui->lineEditID->addStrings(nameSet);
        int initialNameCount = 0;
        QString initialName = QString("Body%1").arg(initialNameCount, 3, 10, QLatin1Char('0'));
        while (nameSet.count(initialName.toStdString()))
        {
            initialNameCount++;
            initialName = QString("Body%1").arg(initialNameCount, 3, 10, QLatin1Char('0'));
            if (initialNameCount >= 999) break; // only do this for the first 999 bodies
        }
        ui->lineEditID->setText(initialName);
    }

    if (m_inputBody)
    {
        double mass, ixx, iyy, izz, ixy, izx, iyz;
        m_inputBody->GetMass(&mass, &ixx, &iyy, &izz, &ixy, &izx, &iyz);
        ui->lineEditMass->setValue(mass);
        ui->lineEditI11->setValue(ixx);
        ui->lineEditI22->setValue(iyy);
        ui->lineEditI33->setValue(izz);
        ui->lineEditI12->setValue(ixy);
        ui->lineEditI13->setValue(izx);
        ui->lineEditI23->setValue(iyz);
        ui->lineEditDensity->setValue(m_inputBody->GetConstructionDensity());
        pgd::Vector3 constructionPosition = m_inputBody->GetConstructionPosition();
        ui->lineEditX->setValue(constructionPosition[0]);
        ui->lineEditY->setValue(constructionPosition[1]);
        ui->lineEditZ->setValue(constructionPosition[2]);
        pgd::Vector3 initialPosition = m_inputBody->GetInitialPosition();
        ui->lineEditRunX->setValue(initialPosition[0]);
        ui->lineEditRunY->setValue(initialPosition[1]);
        ui->lineEditRunZ->setValue(initialPosition[2]);
        pgd::Quaternion initialQuaternion = m_inputBody->GetInitialQuaternion();
        pgd::Vector3 eulerAngles = pgd::MakeEulerAnglesFromQ(pgd::Quaternion(initialQuaternion));
        ui->lineEditEulerX->setValue(eulerAngles.x);
        ui->lineEditEulerY->setValue(eulerAngles.y);
        ui->lineEditEulerZ->setValue(eulerAngles.z);
        pgd::Vector3 angularlinear = m_inputBody->GetLinearVelocity();
        ui->lineEditVX->setValue(angularlinear[0]);
        ui->lineEditVY->setValue(angularlinear[1]);
        ui->lineEditVZ->setValue(angularlinear[2]);
        pgd::Vector3 angularVelocity = m_inputBody->GetAngularVelocity();
        ui->lineEditAVX->setValue(angularVelocity[0]);
        ui->lineEditAVY->setValue(angularVelocity[1]);
        ui->lineEditAVZ->setValue(angularVelocity[2]);
        pgd::Vector3 positionHighBound = m_inputBody->GetPositionHighBound();
        ui->lineEditHighX->setValue(positionHighBound[0]);
        ui->lineEditHighY->setValue(positionHighBound[1]);
        ui->lineEditHighZ->setValue(positionHighBound[2]);
        pgd::Vector3 positionLowBound = m_inputBody->GetPositionLowBound();
        ui->lineEditLowX->setValue(positionLowBound[0]);
        ui->lineEditLowY->setValue(positionLowBound[1]);
        ui->lineEditLowZ->setValue(positionLowBound[2]);
        pgd::Vector3 linearVelocityHighBound = m_inputBody->GetLinearVelocityHighBound();
        ui->lineEditHighVX->setValue(linearVelocityHighBound[0]);
        ui->lineEditHighVY->setValue(linearVelocityHighBound[1]);
        ui->lineEditHighVZ->setValue(linearVelocityHighBound[2]);
        pgd::Vector3 linearVelocityLowBound = m_inputBody->GetLinearVelocityLowBound();
        ui->lineEditLowVX->setValue(linearVelocityLowBound[0]);
        ui->lineEditLowVY->setValue(linearVelocityLowBound[1]);
        ui->lineEditLowVZ->setValue(linearVelocityLowBound[2]);
        pgd::Vector3 angularVelocityHighBound = m_inputBody->GetAngularVelocityHighBound();
        ui->lineEditHighAVX->setValue(angularVelocityHighBound[0]);
        ui->lineEditHighAVY->setValue(angularVelocityHighBound[1]);
        ui->lineEditHighAVZ->setValue(angularVelocityHighBound[2]);
        pgd::Vector3 angularVelocityLowBound = m_inputBody->GetAngularVelocityLowBound();
        ui->lineEditLowAVX->setValue(angularVelocityLowBound[0]);
        ui->lineEditLowAVY->setValue(angularVelocityLowBound[1]);
        ui->lineEditLowAVZ->setValue(angularVelocityLowBound[2]);

        std::string completePath = DialogBodyBuilder::findCompletePath(m_inputBody->GetGraphicFile1());
        if (completePath.size()) ui->lineEditMesh1->setText(QString::fromStdString(completePath));
        else ui->lineEditMesh1->setText(QString::fromStdString(m_inputBody->GetGraphicFile1()));

        completePath = DialogBodyBuilder::findCompletePath(m_inputBody->GetGraphicFile2());
        if (completePath.size()) ui->lineEditMesh2->setText(QString::fromStdString(completePath));
        else ui->lineEditMesh2->setText(QString::fromStdString(m_inputBody->GetGraphicFile2()));

        completePath = DialogBodyBuilder::findCompletePath(m_inputBody->GetGraphicFile3());
        if (completePath.size()) ui->lineEditMesh3->setText(QString::fromStdString(completePath));
        else ui->lineEditMesh3->setText(QString::fromStdString(m_inputBody->GetGraphicFile3()));
    }

    lineEditMeshActivated(ui->lineEditMesh1);

}

void DialogBodyBuilder::accept() // this catches OK and return/enter
{
    qDebug() << "DialogBodyBuilder::accept()";
    GaitSym::Body *bodyPtr;
    if (m_inputBody) bodyPtr = m_inputBody;
    else
    {
        m_outputBody = std::make_unique<GaitSym::Body>(/*m_simulation->GetWorldID()*/);
        bodyPtr = m_outputBody.get();
        bodyPtr->EnterConstructionMode();
    }
    bodyPtr->setName(ui->lineEditID->text().toStdString());
    std::string head, tail;
    pystring::os::path::split(head, tail, ui->lineEditMesh1->text().toStdString());
    bodyPtr->SetGraphicFile1(tail);
    m_simulation->GetGlobal()->MeshSearchPathAddToFront(head);
    pystring::os::path::split(head, tail, ui->lineEditMesh2->text().toStdString());
    bodyPtr->SetGraphicFile2(tail);
    m_simulation->GetGlobal()->MeshSearchPathAddToFront(head);
    pystring::os::path::split(head, tail, ui->lineEditMesh3->text().toStdString());
    bodyPtr->SetGraphicFile3(tail);
    m_simulation->GetGlobal()->MeshSearchPathAddToFront(head);
    bodyPtr->setSimulation(m_simulation);

    double mass = ui->lineEditMass->value();
    double ixx = ui->lineEditI11->value();
    double iyy = ui->lineEditI22->value();
    double izz = ui->lineEditI33->value();
    double ixy = ui->lineEditI12->value();
    double izx = ui->lineEditI13->value();
    double iyz = ui->lineEditI23->value();
    bodyPtr->SetMass(mass, ixx, iyy, izz, ixy, izx, iyz);
    bodyPtr->SetConstructionDensity(ui->lineEditDensity->value());
    pgd::Vector3 constructionPosition;
    constructionPosition[0] = ui->lineEditX->value();
    constructionPosition[1] = ui->lineEditY->value();
    constructionPosition[2] = ui->lineEditZ->value();
    bodyPtr->SetConstructionPosition(constructionPosition[0], constructionPosition[1], constructionPosition[2]);
    pgd::Vector3 positionHighBound;
    positionHighBound[0] = ui->lineEditHighX->value();
    positionHighBound[1] = ui->lineEditHighY->value();
    positionHighBound[2] = ui->lineEditHighZ->value();
    bodyPtr->SetPositionHighBound(positionHighBound[0], positionHighBound[1], positionHighBound[2]);
    pgd::Vector3 positionLowBound;
    positionLowBound[0] = ui->lineEditLowX->value();
    positionLowBound[1] = ui->lineEditLowY->value();
    positionLowBound[2] = ui->lineEditLowZ->value();
    bodyPtr->SetPositionLowBound(positionLowBound[0], positionLowBound[1], positionLowBound[2]);
    pgd::Vector3 linearVelocityHighBound;
    linearVelocityHighBound[0] = ui->lineEditHighVX->value();
    linearVelocityHighBound[1] = ui->lineEditHighVY->value();
    linearVelocityHighBound[2] = ui->lineEditHighVZ->value();
    bodyPtr->SetLinearVelocityHighBound(linearVelocityHighBound[0], linearVelocityHighBound[1], linearVelocityHighBound[2]);
    pgd::Vector3 linearVelocityLowBound;
    linearVelocityLowBound[0] = ui->lineEditLowVX->value();
    linearVelocityLowBound[1] = ui->lineEditLowVY->value();
    linearVelocityLowBound[2] = ui->lineEditLowVZ->value();
    bodyPtr->SetLinearVelocityLowBound(linearVelocityLowBound[0], linearVelocityLowBound[1], linearVelocityLowBound[2]);
    pgd::Vector3 angularVelocityHighBound;
    angularVelocityHighBound[0] = ui->lineEditHighAVX->value();
    angularVelocityHighBound[1] = ui->lineEditHighAVY->value();
    angularVelocityHighBound[2] = ui->lineEditHighAVZ->value();
    bodyPtr->SetAngularVelocityHighBound(angularVelocityHighBound[0], angularVelocityHighBound[1], angularVelocityHighBound[2]);
    pgd::Vector3 angularVelocityLowBound;
    angularVelocityLowBound[0] = ui->lineEditLowAVX->value();
    angularVelocityLowBound[1] = ui->lineEditLowAVY->value();
    angularVelocityLowBound[2] = ui->lineEditLowAVZ->value();
    bodyPtr->SetAngularVelocityLowBound(angularVelocityLowBound[0], angularVelocityLowBound[1], angularVelocityLowBound[2]);

    // and because we are in construction mode we set the position to the construction position/quaternion
    // and the initial position to the desired position/quaternion
    bodyPtr->SetPosition(constructionPosition[0], constructionPosition[1], constructionPosition[2]);
    pgd::Vector3 initialPosition;
    initialPosition[0] = ui->lineEditRunX->value();
    initialPosition[1] = ui->lineEditRunY->value();
    initialPosition[2] = ui->lineEditRunZ->value();
    bodyPtr->SetInitialPosition(initialPosition[0], initialPosition[1], initialPosition[2]);
    double ex = ui->lineEditEulerX->value();
    double ey = ui->lineEditEulerY->value();
    double ez = ui->lineEditEulerZ->value();
    pgd::Quaternion q = pgd::MakeQFromEulerAngles(ex, ey, ez);
    bodyPtr->SetInitialQuaternion(q.n, q.x, q.y, q.z);
    // but the velocities can just be set
    pgd::Vector3 linearVelocity;
    linearVelocity[0] = ui->lineEditVX->value();
    linearVelocity[1] = ui->lineEditVY->value();
    linearVelocity[2] = ui->lineEditVZ->value();
    bodyPtr->SetLinearVelocity(linearVelocity[0], linearVelocity[1], linearVelocity[2]);
    pgd::Vector3 angularVelocity;
    angularVelocity[0] = ui->lineEditAVX->value();
    angularVelocity[1] = ui->lineEditAVY->value();
    angularVelocity[2] = ui->lineEditAVZ->value();
    bodyPtr->SetAngularVelocity(angularVelocity[0], angularVelocity[1], angularVelocity[2]);

    if (m_inputBody)
    {
        bodyPtr->setSize1(m_inputBody->size1());
        bodyPtr->setSize2(m_inputBody->size2());
        bodyPtr->setColour1(m_inputBody->colour1());
        bodyPtr->setColour2(m_inputBody->colour2());
        bodyPtr->setColour3(m_inputBody->colour3());
    }
    else
    {
        bodyPtr->setSize1(Preferences::valueDouble("BodyAxesSize"));
        bodyPtr->setSize2(Preferences::valueDouble("BodyBlendFraction"));
        bodyPtr->setColour1(Preferences::valueQColor("BodyColour1").name(QColor::HexArgb).toStdString());
        bodyPtr->setColour2(Preferences::valueQColor("BodyColour2").name(QColor::HexArgb).toStdString());
        bodyPtr->setColour3(Preferences::valueQColor("BodyColour3").name(QColor::HexArgb).toStdString());
    }

    if (m_properties.size() > 0)
    {
        if (m_properties.count("BodyAxesSize"))
            bodyPtr->setSize1(m_properties["BodyAxesSize"].value.toDouble());
        if (m_properties.count("BodyBlendFraction"))
            bodyPtr->setSize2(m_properties["BodyBlendFraction"].value.toDouble());
        if (m_properties.count("BodyColour1"))
            bodyPtr->setColour1(qvariant_cast<QColor>(m_properties["BodyColour1"].value).name(QColor::HexArgb).toStdString());
        if (m_properties.count("BodyColour2"))
            bodyPtr->setColour2(qvariant_cast<QColor>(m_properties["BodyColour2"].value).name(QColor::HexArgb).toStdString());
        if (m_properties.count("BodyColour3"))
            bodyPtr->setColour3(qvariant_cast<QColor>(m_properties["BodyColour3"].value).name(QColor::HexArgb).toStdString());
    }

    // this is needed because there are some parts of GaitSym::Body that do not have a public interface
    bodyPtr->saveToAttributes();
    bodyPtr->createFromAttributes();

    Preferences::insert("DialogBodyBuilderMoveMarkers", ui->checkBoxMoveMarkers->isChecked());
    Preferences::insert("DialogBodyBuilderGeometry", saveGeometry());

    QDialog::accept();
}

void DialogBodyBuilder::reject() // this catches cancel, close and escape key
{
    qDebug() << "DialogBodyBuilder::reject()";
    Preferences::insert("DialogBodyBuilderMoveMarkers", ui->checkBoxMoveMarkers->isChecked());
    Preferences::insert("DialogBodyBuilderGeometry", saveGeometry());

    QDialog::reject();
}

void DialogBodyBuilder::closeEvent(QCloseEvent *event)
{
    qDebug() << "DialogBodyBuilder::closeEvent()";
    Preferences::insert("DialogBodyBuilderMoveMarkers", ui->checkBoxMoveMarkers->isChecked());
    Preferences::insert("DialogBodyBuilderGeometry", saveGeometry());

    QDialog::closeEvent(event);
}

void DialogBodyBuilder::calculate()
{
    if (!m_referenceObject) return;
    double density = ui->lineEditDensity->text().toDouble();
    bool clockwise = false;
    pgd::Vector3 translation;
    double mass;
    pgd::Vector3 centreOfMass;
    pgd::Matrix3x3 inertialTensor;
    m_referenceObject->CalculateMassProperties(density, clockwise, translation, &mass, &centreOfMass, &inertialTensor);
    ui->lineEditMass->setValue(mass);
    ui->lineEditX->setValue(centreOfMass[0]);
    ui->lineEditY->setValue(centreOfMass[1]);
    ui->lineEditZ->setValue(centreOfMass[2]);
    // now recalculate the inertial tensor arount the centre of mass
    translation.Set(-centreOfMass[0], -centreOfMass[1], -centreOfMass[2]);
    m_referenceObject->CalculateMassProperties(density, clockwise, translation, &mass, &centreOfMass, &inertialTensor);
    double ixx, iyy, izz, ixy, izx, iyz;
    inertialTensor.GetInertia(&ixx, &iyy, &izz, &ixy, &izx, &iyz);
    ui->lineEditI11->setValue(ixx);
    ui->lineEditI22->setValue(iyy);
    ui->lineEditI33->setValue(izz);
    ui->lineEditI12->setValue(ixy);
    ui->lineEditI13->setValue(izx);
    ui->lineEditI23->setValue(iyz);
}

void DialogBodyBuilder::lineEditMeshClicked()
{
    LineEditPath *lineEdit = qobject_cast<LineEditPath *>(sender());
    if (lineEdit == nullptr) return;
    lineEditMeshActivated(lineEdit);
}

void DialogBodyBuilder::lineEditMeshActivated(LineEditPath *lineEdit)
{
    while (true)
    {
        if (lineEdit == ui->lineEditMesh1)
        {
            if (lineEdit->text().toStdString() == m_mesh1.filename())
            {
                m_referenceObject = &m_mesh1;
            }
            else
            {
                int err = m_mesh1.ParseMeshFile(lineEdit->text().toStdString());
                if (err) m_referenceObject = nullptr;
                else m_referenceObject = &m_mesh1;
            }
            break;
        }
        if (lineEdit == ui->lineEditMesh2)
        {
            if (lineEdit->text().toStdString() == m_mesh2.filename())
            {
                m_referenceObject = &m_mesh2;
            }
            else
            {
                int err = m_mesh2.ParseMeshFile(lineEdit->text().toStdString());
                if (err) m_referenceObject = nullptr;
                else m_referenceObject = &m_mesh2;
            }
            break;
        }
        if (lineEdit == ui->lineEditMesh3)
        {
            if (lineEdit->text().toStdString() == m_mesh3.filename())
            {
                m_referenceObject = &m_mesh3;
            }
            else
            {
                int err = m_mesh3.ParseMeshFile(lineEdit->text().toStdString());
                if (err) m_referenceObject = nullptr;
                else m_referenceObject = &m_mesh3;
            }
            break;
        }
    }

    ui->pushButtonCalculate->setEnabled(m_referenceObject ? true:false);
    ui->lineEditMesh1->setHighlighted(false);
    ui->lineEditMesh2->setHighlighted(false);
    ui->lineEditMesh3->setHighlighted(false);
    lineEdit->setHighlighted(true);
}


void DialogBodyBuilder::lineEditIDTextChanged(const QString & /* text */)
{
    LineEditUniqueName *lineEdit = qobject_cast<LineEditUniqueName *>(sender());
    if (lineEdit == nullptr) return;
    QString textCopy = lineEdit->text();
    int pos = lineEdit->cursorPosition();
    ui->pushButtonOK->setEnabled(lineEdit->validator()->validate(textCopy, pos) == QValidator::Acceptable);
}

void DialogBodyBuilder::properties()
{
    DialogProperties dialogProperties(this);

    SettingsItem bodyAxesSize = Preferences::settingsItem("BodyAxesSize");
    SettingsItem bodyBlendFraction = Preferences::settingsItem("BodyBlendFraction");
    SettingsItem bodyColour1 = Preferences::settingsItem("BodyColour1");
    SettingsItem bodyColour2 = Preferences::settingsItem("BodyColour2");
    SettingsItem bodyColour3 = Preferences::settingsItem("BodyColour3");

    if (m_inputBody)
    {
        bodyAxesSize.value = m_inputBody->size1();
        bodyBlendFraction.value = m_inputBody->size2();
        bodyColour1.value = QColor(QString::fromStdString(m_inputBody->colour1().GetHexARGB()));
        bodyColour2.value = QColor(QString::fromStdString(m_inputBody->colour2().GetHexARGB()));
        bodyColour3.value = QColor(QString::fromStdString(m_inputBody->colour3().GetHexARGB()));
    }
    m_properties.clear();
    m_properties = { { bodyAxesSize.key, bodyAxesSize },
                     { bodyBlendFraction.key, bodyBlendFraction },
                     { bodyColour1.key, bodyColour1 },
                     { bodyColour2.key, bodyColour2 },
                     { bodyColour3.key, bodyColour3 } };
    dialogProperties.setInputSettingsItems(m_properties);
    dialogProperties.initialise();

    int status = dialogProperties.exec();
    if (status == QDialog::Accepted)
    {
        dialogProperties.update();
        m_properties = dialogProperties.getOutputSettingsItems();
    }
}

std::string DialogBodyBuilder::findCompletePath(const std::string &filename)
{
    std::string completePath;
    if (m_simulation)
    {
        auto searchPath = m_simulation->GetGlobal()->MeshSearchPath();
        for (auto &&it : *searchPath)
        {
            completePath = pystring::os::path::join(it, filename);
            if (QFileInfo(QString::fromStdString(completePath)).isFile()) return completePath;
        }
    }
    return std::string();
}

std::unique_ptr<GaitSym::Body> DialogBodyBuilder::outputBody()
{
    return std::move(m_outputBody);
}

void DialogBodyBuilder::setInputBody(GaitSym::Body *inputBody)
{
    m_inputBody = inputBody;
}

GaitSym::Simulation *DialogBodyBuilder::simulation() const
{
    return m_simulation;
}

void DialogBodyBuilder::setSimulation(GaitSym::Simulation *simulation)
{
    m_simulation = simulation;
}



