#include "DialogGeoms.h"
#include "ui_DialogGeoms.h"

#include "Geom.h"
#include "Simulation.h"
#include "Preferences.h"
#include "SphereGeom.h"
#include "CappedCylinderGeom.h"
#include "BoxGeom.h"
#include "PlaneGeom.h"
#include "ConvexGeom.h"
#include "TrimeshGeom.h"
#include "GSUtil.h"
#include "Marker.h"
#include "DialogProperties.h"
#include "MainWindow.h"
#include "PGDMath.h"

#include "pystring.h"

#include <QDebug>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>

#include <algorithm>


DialogGeoms::DialogGeoms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGeoms)
{
    ui->setupUi(this);
    MainWindow::layoutSpacing(this);

    setWindowTitle(tr("Geom Builder"));
#ifdef Q_OS_MACOS
    setWindowFlags(windowFlags() & (~Qt::Dialog) | Qt::Window); // allows the window to be resized on macs
#endif
    restoreGeometry(Preferences::valueQByteArray("DialogGeomsGeometry"));

    QVBoxLayout *verticalLayoutExcludedGeoms;
    QScrollArea *scrollAreaExcludedGeoms;
    QWidget *scrollAreaWidgetContentsExcludedGeoms;
    verticalLayoutExcludedGeoms = new QVBoxLayout();
    verticalLayoutExcludedGeoms->setSpacing(6);
    verticalLayoutExcludedGeoms->setContentsMargins(11, 11, 11, 11);
    verticalLayoutExcludedGeoms->setObjectName(QStringLiteral("verticalLayout"));
    scrollAreaExcludedGeoms = new QScrollArea();
    scrollAreaExcludedGeoms->setObjectName(QStringLiteral("scrollArea"));
    scrollAreaExcludedGeoms->setWidgetResizable(true);
    scrollAreaWidgetContentsExcludedGeoms = new QWidget();
    scrollAreaWidgetContentsExcludedGeoms->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
    m_gridLayoutExcludedGeoms = new QGridLayout();
    m_gridLayoutExcludedGeoms->setSpacing(6);
    m_gridLayoutExcludedGeoms->setContentsMargins(11, 11, 11, 11);
    m_gridLayoutExcludedGeoms->setObjectName(QStringLiteral("gridLayout"));
    scrollAreaWidgetContentsExcludedGeoms->setLayout(m_gridLayoutExcludedGeoms);
    scrollAreaExcludedGeoms->setWidget(scrollAreaWidgetContentsExcludedGeoms);
    verticalLayoutExcludedGeoms->addWidget(scrollAreaExcludedGeoms);
    ui->widgetExcludedGeomsPlaceholder->setLayout(verticalLayoutExcludedGeoms);

    connect(ui->pushButtonOK, &QPushButton::clicked, this, &DialogGeoms::accept);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &DialogGeoms::reject);
    connect(ui->pushButtonProperties, &QPushButton::clicked, this, &DialogGeoms::properties);
    connect(ui->pushButtonImportOBJConvex, &QPushButton::clicked, this, &DialogGeoms::importOBJConvex);
    connect(ui->pushButtonImportOBJTrimesh, &QPushButton::clicked, this, &DialogGeoms::importOBJTrimesh);

    // this logic monitors for changing values
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    for (auto it = widgets.begin(); it != widgets.end(); it++)
    {
        if (QComboBox *comboBox = dynamic_cast<QComboBox *>(*it)) connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DialogGeoms::comboBoxChanged); // QOverload<int> selects the (int) rather than the (QString) version of currentIndexChanged
        if (QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(*it)) connect(lineEdit, &QLineEdit::textChanged, this, &DialogGeoms::lineEditChanged);
        if (QSpinBox *spinBox = dynamic_cast<QSpinBox *>(*it)) connect(spinBox, &QSpinBox::textChanged, this, &DialogGeoms::spinBoxChanged);
        if (QCheckBox *checkBox = dynamic_cast<QCheckBox *>(*it)) connect(checkBox, &QCheckBox::checkStateChanged, this, &DialogGeoms::checkBoxStateChanged);
    }

}

DialogGeoms::~DialogGeoms()
{
    delete ui;
}

void DialogGeoms::accept() // this catches OK and return/enter
{
    qDebug() << "DialogGeoms::accept()";

    std::map<std::string, std::unique_ptr<GaitSym::Marker>> *markerList = m_simulation->GetMarkerList();
    QString strapTab = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    while (true)
    {
        if (strapTab == "Sphere")
        {
            m_outputGeom = std::make_unique<GaitSym::SphereGeom>(ui->lineEditSphereRadius->value());
            break;
        }
        if (strapTab == "Capsule")
        {
            m_outputGeom = std::make_unique<GaitSym::CappedCylinderGeom>(ui->lineEditCapsuleRadius->value(), ui->lineEditCapsuleLength->value());
            break;
        }
        if (strapTab == "Box")
        {
            m_outputGeom = std::make_unique<GaitSym::BoxGeom>(ui->lineEditBoxLengthX->value(), ui->lineEditBoxLengthY->value(), ui->lineEditBoxLengthZ->value());
            break;
        }
        if (strapTab == "Plane")
        {
            GaitSym::Marker *geomMarker = markerList->at(ui->comboBoxGeomMarker->currentText().toStdString()).get();
            pgd::Vector3 normal = geomMarker->GetWorldAxis(GaitSym::Marker::Axis::Z);
            pgd::Vector3 point = geomMarker->GetWorldPosition();
            double a = normal.x;
            double b = normal.y;
            double c = normal.z;
            double d = normal.Dot(point);
            m_outputGeom = std::make_unique<GaitSym::PlaneGeom>(a, b, c, d);
            break;
        }
        if (strapTab == "Convex")
        {
            auto convexGeom = std::make_unique<GaitSym::ConvexGeom>();
            convexGeom->setReverseWinding(ui->checkBoxReverseWindingConvex->isChecked());
            convexGeom->setIndexStart(ui->spinBoxIndexStartConvex->value());
            std::string triangleIndicesString = ui->plainTextEditTriangleIndicesConvex->toPlainText().toStdString();
            std::vector<int> *triangles = convexGeom->triangles();
            GaitSym::GSUtil::Int(triangleIndicesString, triangles);
            std::string verticesString = ui->plainTextEditVerticesConvex->toPlainText().toStdString();
            std::vector<double> *vertices = convexGeom->vertices();
            GaitSym::GSUtil::Double(verticesString, vertices);
            m_outputGeom = std::move(convexGeom);
            break;
        }
        if (strapTab == "Trimesh")
        {
            auto trimeshGeom = std::make_unique<GaitSym::TrimeshGeom>();
            trimeshGeom->setReverseWinding(ui->checkBoxReverseWindingTrimesh->isChecked());
            trimeshGeom->setIndexStart(ui->spinBoxIndexStartTrimesh->value());
            std::string triangleIndicesString = ui->plainTextEditTriangleIndicesTrimesh->toPlainText().toStdString();
            std::vector<int> *triangles = trimeshGeom->triangles();
            GaitSym::GSUtil::Int(triangleIndicesString, triangles);
            std::string verticesString = ui->plainTextEditVerticesTrimesh->toPlainText().toStdString();
            std::vector<double> *vertices = trimeshGeom->vertices();
            GaitSym::GSUtil::Double(verticesString, vertices);
            m_outputGeom = std::move(trimeshGeom);
            break;
        }

        qDebug() << "DialogGeoms::accept() strap type error";
    }

    m_outputGeom->setName(ui->lineEditGeomID->text().toStdString());
    m_outputGeom->setSimulation(m_simulation);
    m_outputGeom->setGeomMarker(markerList->at(ui->comboBoxGeomMarker->currentText().toStdString()).get());
    m_outputGeom->SetSpringDamp(ui->lineEditSpring->value(), ui->lineEditDamp->value(), m_simulation->GetTimeIncrement());
    m_outputGeom->SetContactMu(ui->lineEditMu->value());
    m_outputGeom->SetContactRho(ui->lineEditRho->value());
    m_outputGeom->SetContactBounce(ui->lineEditBounce->value());
    m_outputGeom->SetAbort(ui->checkBoxAbort->isChecked());

    std::vector<GaitSym::Geom *> *excludedGeoms = m_outputGeom->GetExcludeList();
    excludedGeoms->clear();
    if (ui->spinBoxNExcludedGeoms->value())
    {
        auto geomList = m_simulation->GetGeomList();
        for (int i = 0; i < ui->spinBoxNExcludedGeoms->value(); i++)
        {
            GaitSym::Geom *geom = geomList->at(m_excludedGeomComboBoxList[i]->currentText().toStdString()).get();
            if (std::find(excludedGeoms->begin(), excludedGeoms->end(), geom) == excludedGeoms->end() && geom->name() != m_outputGeom->name())
                excludedGeoms->push_back(geom);
        }
    }

    if (m_inputGeom)
    {
        m_outputGeom->setColour1(m_inputGeom->colour1());
        m_outputGeom->setColour2(m_inputGeom->colour2());
        m_outputGeom->setSize1(m_inputGeom->size1());
        m_outputGeom->setSize2(m_inputGeom->size2());
        m_outputGeom->setSize3(m_inputGeom->size3());
    }
    else
    {
        m_outputGeom->setColour1(Preferences::valueQColor("GeomColour1").name(QColor::HexArgb).toStdString());
        m_outputGeom->setColour2(Preferences::valueQColor("GeomColour2").name(QColor::HexArgb).toStdString());
        m_outputGeom->setSize1(Preferences::valueDouble("GeomSize1"));
        m_outputGeom->setSize2(Preferences::valueDouble("GeomSize2"));
        m_outputGeom->setSize3(Preferences::valueDouble("GeomSize3"));
    }

    if (m_properties.size() > 0)
    {
        if (m_properties.count("GeomColour1"))
            m_outputGeom->setColour1(qvariant_cast<QColor>(m_properties["GeomColour1"].value).name(QColor::HexArgb).toStdString());
        if (m_properties.count("GeomColour2"))
            m_outputGeom->setColour2(qvariant_cast<QColor>(m_properties["GeomColour2"].value).name(QColor::HexArgb).toStdString());
        if (m_properties.count("GeomSize1"))
            m_outputGeom->setSize1(m_properties["GeomSize1"].value.toDouble());
        if (m_properties.count("GeomSize2"))
            m_outputGeom->setSize2(m_properties["GeomSize2"].value.toDouble());
        if (m_properties.count("GeomSize3"))
            m_outputGeom->setSize3(m_properties["GeomSize3"].value.toDouble());
    }

    m_outputGeom->saveToAttributes();
    m_outputGeom->createFromAttributes();

    Preferences::insert("DialogGeomsGeometry", saveGeometry());
    QDialog::accept();
}

void DialogGeoms::reject() // this catches cancel, close and escape key
{
    qDebug() << "DialogGeoms::reject()";
    Preferences::insert("DialogGeomsGeometry", saveGeometry());
    QDialog::reject();
}

void DialogGeoms::closeEvent(QCloseEvent *event)
{
    Preferences::insert("DialogGeomsGeometry", saveGeometry());

    QDialog::closeEvent(event);
}

void DialogGeoms::lateInitialise()
{
    Q_ASSERT_X(m_simulation, "DialogGeoms::lateInitialise", "m_simulation undefined");

    // set the marker lists
    QStringList markerIDs;
    for (auto &&it : *m_simulation->GetMarkerList()) markerIDs.append(QString::fromStdString(it.first));
    ui->comboBoxGeomMarker->addItems(markerIDs);

    // now set some sensible defaults
    ui->lineEditSpring->setBottom(std::numeric_limits<double>::min());
    ui->lineEditSpring->setValue(Preferences::valueDouble("GlobalDefaultSpringConstant"));
    ui->lineEditDamp->setBottom(std::numeric_limits<double>::min());
    ui->lineEditDamp->setValue(Preferences::valueDouble("GlobalDefaultDampingConstant"));
    ui->lineEditBounce->setBottom(-1);
    ui->lineEditBounce->setValue(-1);
    ui->lineEditMu->setBottom(-1);
    ui->lineEditMu->setValue(1);
    ui->lineEditRho->setBottom(-1);
    ui->lineEditRho->setValue(-1);
    ui->checkBoxAbort->setChecked(false);
    ui->checkBoxAdhesion->setChecked(false);

    ui->lineEditSphereRadius->setBottom(std::numeric_limits<double>::min());
    ui->lineEditSphereRadius->setValue(1);
    ui->lineEditCapsuleRadius->setBottom(std::numeric_limits<double>::min());
    ui->lineEditCapsuleRadius->setValue(1);
    ui->lineEditCapsuleLength->setBottom(std::numeric_limits<double>::min());
    ui->lineEditCapsuleLength->setValue(1);
    ui->lineEditBoxLengthX->setBottom(std::numeric_limits<double>::min());
    ui->lineEditBoxLengthX->setValue(1);
    ui->lineEditBoxLengthY->setBottom(std::numeric_limits<double>::min());
    ui->lineEditBoxLengthY->setValue(1);
    ui->lineEditBoxLengthZ->setBottom(std::numeric_limits<double>::min());
    ui->lineEditBoxLengthZ->setValue(1);

    if (!m_inputGeom)
    {
        // set default new name
        auto nameSet = simulation()->GetNameSet();
        ui->lineEditGeomID->addStrings(nameSet);
        int initialNameCount = 0;
        QString initialName = QString("Geom%1").arg(initialNameCount, 3, 10, QLatin1Char('0'));
        while (nameSet.count(initialName.toStdString()))
        {
            initialNameCount++;
            initialName = QString("Geom%1").arg(initialNameCount, 3, 10, QLatin1Char('0'));
            if (initialNameCount >= 999) break; // only do this for the first 999 markers
        }
        ui->lineEditGeomID->setText(initialName);
        ui->tabWidget->setCurrentIndex(0);
        return;
    }

    QStringList tabNames;
    for (int i = 0; i < ui->tabWidget->count(); i++) tabNames.push_back(ui->tabWidget->tabText(i));

    std::string s;
    m_inputGeom->saveToAttributes();
    ui->lineEditGeomID->setText(QString::fromStdString(m_inputGeom->findAttribute("ID"s)));
    ui->lineEditGeomID->setEnabled(false);
    ui->comboBoxGeomMarker->setCurrentText(QString::fromStdString(m_inputGeom->findAttribute("MarkerID"s)));
    if ((s = m_inputGeom->findAttribute("SpringConstant"s)).size()) ui->lineEditSpring->setValue(GaitSym::GSUtil::Double(s));
    if ((s = m_inputGeom->findAttribute("DampingConstant"s)).size()) ui->lineEditDamp->setValue(GaitSym::GSUtil::Double(s));
    if ((s = m_inputGeom->findAttribute("Bounce"s)).size()) ui->lineEditBounce->setValue(GaitSym::GSUtil::Double(s));
    if ((s = m_inputGeom->findAttribute("Mu"s)).size()) ui->lineEditMu->setValue(GaitSym::GSUtil::Double(s));
    if ((s = m_inputGeom->findAttribute("Rho"s)).size()) ui->lineEditRho->setValue(GaitSym::GSUtil::Double(s));
    if ((s = m_inputGeom->findAttribute("Abort"s)).size()) ui->checkBoxAbort->setChecked(GaitSym::GSUtil::Bool(s));
    if ((s = m_inputGeom->findAttribute("Adhesion"s)).size()) ui->checkBoxAdhesion->setChecked(GaitSym::GSUtil::Bool(s));

    std::vector<GaitSym::Geom *> *excludeList = m_inputGeom->GetExcludeList();
    if (excludeList->size())
    {
        QStringList geomIDs;
        for (auto &&it : *m_simulation->GetGeomList()) geomIDs.append(QString::fromStdString(it.first));
        const QSignalBlocker blocker(ui->spinBoxNExcludedGeoms);
        ui->spinBoxNExcludedGeoms->setValue(int(excludeList->size()));
        for (int i = 0; i < ui->spinBoxNExcludedGeoms->value(); i++)
        {
            QLabel *label = new QLabel();
            label->setText(QString("Excluded Geom %1").arg(i));
            m_gridLayoutExcludedGeoms->addWidget(label, i, 0, Qt::AlignTop);
            QComboBox *comboBoxMarker = new QComboBox();
            comboBoxMarker->addItems(geomIDs);
            comboBoxMarker->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            m_gridLayoutExcludedGeoms->addWidget(comboBoxMarker, i, 1, Qt::AlignTop);
            m_excludedGeomLabelList.push_back(label);
            m_excludedGeomComboBoxList.push_back(comboBoxMarker);
            comboBoxMarker->setCurrentText(QString::fromStdString(excludeList->at(size_t(i))->name()));
        }
        QSpacerItem *gridSpacerExcludedGeoms = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        m_gridLayoutExcludedGeoms->addItem(gridSpacerExcludedGeoms, ui->spinBoxNExcludedGeoms->value(), 0);
    }

    if (GaitSym::SphereGeom *sphereGeom = dynamic_cast<GaitSym::SphereGeom *>(m_inputGeom))
    {
        if ((s = sphereGeom->findAttribute("Radius"s)).size()) ui->lineEditSphereRadius->setValue(GaitSym::GSUtil::Double(s));
        ui->tabWidget->setCurrentIndex(tabNames.indexOf("Sphere"));
    }

    if (GaitSym::CappedCylinderGeom *cappedCylinderGeom = dynamic_cast<GaitSym::CappedCylinderGeom *>(m_inputGeom))
    {
        if ((s = cappedCylinderGeom->findAttribute("Radius"s)).size()) ui->lineEditCapsuleRadius->setValue(GaitSym::GSUtil::Double(s));
        if ((s = cappedCylinderGeom->findAttribute("Length"s)).size()) ui->lineEditCapsuleLength->setValue(GaitSym::GSUtil::Double(s));
        ui->tabWidget->setCurrentIndex(tabNames.indexOf("Capsule"));
    }

    if (GaitSym::BoxGeom *boxGeom = dynamic_cast<GaitSym::BoxGeom *>(m_inputGeom))
    {
        if ((s = boxGeom->findAttribute("LengthX"s)).size()) ui->lineEditBoxLengthX->setValue(GaitSym::GSUtil::Double(s));
        if ((s = boxGeom->findAttribute("LengthY"s)).size()) ui->lineEditBoxLengthY->setValue(GaitSym::GSUtil::Double(s));
        if ((s = boxGeom->findAttribute("LengthZ"s)).size()) ui->lineEditBoxLengthZ->setValue(GaitSym::GSUtil::Double(s));
        ui->tabWidget->setCurrentIndex(tabNames.indexOf("Box"));
    }

    if (GaitSym::PlaneGeom *planeGeom = dynamic_cast<GaitSym::PlaneGeom *>(m_inputGeom))
    {
        (void)planeGeom; // Silences unused warning
        ui->tabWidget->setCurrentIndex(tabNames.indexOf("Plane"));
    }

    if (GaitSym::TrimeshGeom *trimeshGeom = dynamic_cast<GaitSym::TrimeshGeom *>(m_inputGeom))
    {
        if ((s = trimeshGeom->findAttribute("IndexStart"s)).size()) ui->spinBoxIndexStartTrimesh->setValue(GaitSym::GSUtil::Int(s));
        if ((s = trimeshGeom->findAttribute("ReverseWinding"s)).size()) ui->checkBoxReverseWindingTrimesh->setChecked(GaitSym::GSUtil::Bool(s));
        if ((s = trimeshGeom->findAttribute("Vertices"s)).size()) ui->plainTextEditVerticesTrimesh->setPlainText(QString::fromStdString(listToLines(pystring::split(s), 3)));
        if ((s = trimeshGeom->findAttribute("Triangles"s)).size()) ui->plainTextEditTriangleIndicesTrimesh->setPlainText(QString::fromStdString(listToLines(pystring::split(s), 3)));
        ui->tabWidget->setCurrentIndex(tabNames.indexOf("Trimesh"));
    }

    if (GaitSym::ConvexGeom *convexGeom = dynamic_cast<GaitSym::ConvexGeom *>(m_inputGeom))
    {
        if ((s = convexGeom->findAttribute("IndexStart"s)).size()) ui->spinBoxIndexStartConvex->setValue(GaitSym::GSUtil::Int(s));
        if ((s = convexGeom->findAttribute("ReverseWinding"s)).size()) ui->checkBoxReverseWindingConvex->setChecked(GaitSym::GSUtil::Bool(s));
        if ((s = convexGeom->findAttribute("Vertices"s)).size()) ui->plainTextEditVerticesConvex->setPlainText(QString::fromStdString(s));
        if ((s = convexGeom->findAttribute("Triangles"s)).size()) ui->plainTextEditTriangleIndicesConvex->setPlainText(QString::fromStdString(s));
        ui->tabWidget->setCurrentIndex(tabNames.indexOf("Convex"));
    }

}

void DialogGeoms::comboBoxChanged(int /*index*/)
{
    updateActivation();
}

void DialogGeoms::lineEditChanged(const QString &/*text*/)
{
    updateActivation();
}

void DialogGeoms::spinBoxChanged(const QString &/*text*/)
{
    if (this->sender() == ui->spinBoxNExcludedGeoms)
    {
        // get the lists in the right formats
        QStringList geomIDs;
        for (auto &&it : *m_simulation->GetGeomList()) geomIDs.append(QString::fromStdString(it.first));

        // store the current values in the list
        QVector<QString> oldValues(m_excludedGeomComboBoxList.size());
        for (int i = 0; i < m_excludedGeomComboBoxList.size(); i++) oldValues[i] = m_excludedGeomComboBoxList[i]->currentText();

        // delete all the existing widgets in the layout
        QLayoutItem *child;
        while ((child = m_gridLayoutExcludedGeoms->takeAt(0)) != nullptr)
        {
            delete child->widget(); // delete the widget
            delete child;   // delete the layout item
        }
        m_excludedGeomLabelList.clear();
        m_excludedGeomComboBoxList.clear();

        // now create a new set
        int requiredExcludedGeoms = ui->spinBoxNExcludedGeoms->value();
        for (int i = 0; i < requiredExcludedGeoms; i++)
        {
            QLabel *label = new QLabel();
            label->setText(QString("Excluded Geom %1").arg(i));
            m_gridLayoutExcludedGeoms->addWidget(label, i, 0, Qt::AlignTop);
            QComboBox *comboBoxMarker = new QComboBox();
            comboBoxMarker->addItems(geomIDs);
            comboBoxMarker->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            if (i < oldValues.size()) comboBoxMarker->setCurrentText(oldValues[i]);
            m_gridLayoutExcludedGeoms->addWidget(comboBoxMarker, i, 1, Qt::AlignTop);
            m_excludedGeomLabelList.push_back(label);
            m_excludedGeomComboBoxList.push_back(comboBoxMarker);
        }
        QSpacerItem *gridSpacerExcludedGeoms = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        m_gridLayoutExcludedGeoms->addItem(gridSpacerExcludedGeoms, requiredExcludedGeoms, 0);
    }
    updateActivation();
}

void DialogGeoms::checkBoxStateChanged(Qt::CheckState /*state*/)
{
    updateActivation();
}

void DialogGeoms::updateActivation()
{
    bool okEnable = true;
    QString textCopy = ui->lineEditGeomID->text();
    int pos = ui->lineEditGeomID->cursorPosition();
    if (ui->lineEditGeomID->validator()->validate(textCopy, pos) != QValidator::Acceptable) okEnable = false;

    ui->pushButtonOK->setEnabled(okEnable);
}

void DialogGeoms::properties()
{
    DialogProperties dialogProperties(this);

    SettingsItem geomColour1 = Preferences::settingsItem("GeomColour1");
    SettingsItem geomColour2 = Preferences::settingsItem("GeomColour2");
    SettingsItem geomSize1 = Preferences::settingsItem("GeomSize1");
    SettingsItem geomSize2 = Preferences::settingsItem("GeomSize2");
    SettingsItem geomSize3 = Preferences::settingsItem("GeomSize3");

    if (m_inputGeom)
    {
        geomColour1.value = QColor(QString::fromStdString(m_inputGeom->colour1().GetHexARGB()));
        geomColour2.value = QColor(QString::fromStdString(m_inputGeom->colour2().GetHexARGB()));
        geomSize1.value = m_inputGeom->size1();
        geomSize2.value = m_inputGeom->size2();
        geomSize3.value = m_inputGeom->size3();
    }
    m_properties.clear();
    m_properties = { { geomColour1.key, geomColour1 },
                     { geomColour2.key, geomColour2 },
                     { geomSize1.key, geomSize1 },
                     { geomSize2.key, geomSize2 },
                     { geomSize3.key, geomSize3 } };

    dialogProperties.setInputSettingsItems(m_properties);
    dialogProperties.initialise();

    int status = dialogProperties.exec();
    if (status == QDialog::Accepted)
    {
        dialogProperties.update();
        m_properties = dialogProperties.getOutputSettingsItems();
    }
}

void DialogGeoms::importOBJConvex()
{
    QFileInfo info(Preferences::valueQString("DialogGeomsLastImportOBJConvex"));
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Import OBJ File"), info.absoluteFilePath(), tr("OBJ Files (*.obj);;Any File (*.* *)"), nullptr);
    if (fileName.isNull() == false)
    {
        Preferences::insert("DialogGeomsLastImportOBJConvex", fileName);
        std::vector<std::string> vertexCoordinates, triangleIndices;
        int status = simplifiedOBJFileReader(fileName.toStdString(), &vertexCoordinates, &triangleIndices);
        if (status)
        {
            QMessageBox::warning(this, "Error parsing OBJ file", QString("'%1'").arg(fileName));
            return;
        }
        std::string vertices = listToLines(vertexCoordinates, 3);
        std::string triangleIndexes = listToLines(triangleIndices, 3);
        ui->spinBoxIndexStartConvex->setValue(1);
        ui->checkBoxReverseWindingConvex->setChecked(false);
        ui->plainTextEditVerticesConvex->setPlainText(QString::fromStdString(vertices));
        ui->plainTextEditTriangleIndicesConvex->setPlainText(QString::fromStdString(triangleIndexes));
    }
}

void DialogGeoms::importOBJTrimesh()
{
    QFileInfo info(Preferences::valueQString("DialogGeomsLastImportOBJTrimesh"));
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Import OBJ File"), info.absoluteFilePath(), tr("OBJ Files (*.obj);;Any File (*.* *)"), nullptr);
    if (fileName.isNull() == false)
    {
        Preferences::insert("DialogGeomsLastImportOBJTrimesh", fileName);
        std::vector<std::string> vertexCoordinates, triangleIndices;
        int status = simplifiedOBJFileReader(fileName.toStdString(), &vertexCoordinates, &triangleIndices);
        if (status)
        {
            QMessageBox::warning(this, "Error parsing OBJ file", QString("'%1'").arg(fileName));
            return;
        }
        std::string vertices = listToLines(vertexCoordinates, 3);
        std::string triangleIndexes = listToLines(triangleIndices, 3);
        ui->spinBoxIndexStartTrimesh->setValue(1);
        ui->checkBoxReverseWindingTrimesh->setChecked(false);
        ui->plainTextEditVerticesTrimesh->setPlainText(QString::fromStdString(vertices));
        ui->plainTextEditTriangleIndicesTrimesh->setPlainText(QString::fromStdString(triangleIndexes));
    }
}

int DialogGeoms::simplifiedOBJFileReader(std::string filename, std::vector<std::string> *vertexCoordinates, std::vector<std::string> *triangleIndices)
{
    // this block of code reads athe file into a std::string
    std::ifstream file(filename, std::ios::binary);  // Open file in binary mode
    if (!file) { return __LINE__; }
    file.seekg(0, std::ios::end); // Move to the end to determine file size
    std::size_t size = file.tellg();
    std::string content(size, '\0');  // Allocate string with exact size
    file.seekg(0); // Move back to the beginning and read the content
    file.read(&content[0], size);

    std::vector<std::string> lines = pystring::splitlines(content);
    std::vector<std::string> tokens;
    for (auto &&line : lines)
    {
        if (pystring::startswith(line, "v "))
        {
            tokens = pystring::split(line);
            if (tokens.size() != 4) { return __LINE__; }
            vertexCoordinates->push_back(tokens[1]);
            vertexCoordinates->push_back(tokens[2]);
            vertexCoordinates->push_back(tokens[3]);
        }
        if (pystring::startswith(line, "f "))
        {
            tokens = pystring::split(line);
            if (tokens.size() != 4) { return __LINE__; }
            triangleIndices->push_back(pystring::split(tokens[1], "/"s)[0]);
            triangleIndices->push_back(pystring::split(tokens[2], "/"s)[0]);
            triangleIndices->push_back(pystring::split(tokens[3], "/"s)[0]);
        }
    }
    return 0;
}

std::string DialogGeoms::listToLines(const std::vector<std::string> &list, size_t elementsPerLine)
{
    std::string output;
    for (size_t i = 0; i < list.size() - 1; ++i)
    {
        size_t column = i % elementsPerLine;
        if (column != elementsPerLine - 1) { output.append(list[i] + " "s); }
        else { output.append(list[i] + "\n"s); }
    }
    output.append(list.back() + "\n"s);
    return output;
}

GaitSym::Simulation *DialogGeoms::simulation() const
{
    return m_simulation;
}

void DialogGeoms::setSimulation(GaitSym::Simulation *simulation)
{
    m_simulation = simulation;
}

std::unique_ptr<GaitSym::Geom> DialogGeoms::outputGeom()
{
    return std::move(m_outputGeom);
}

void DialogGeoms::setInputGeom(GaitSym::Geom *inputGeom)
{
    m_inputGeom = inputGeom;
}

