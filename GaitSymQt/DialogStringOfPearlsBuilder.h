#ifndef DIALOGSTRINGOFPEARLSBUILDER_H
#define DIALOGSTRINGOFPEARLSBUILDER_H

#include "Simulation.h"
#include "Preferences.h"
#include "Body.h"
#include "Marker.h"
#include "Geom.h"
#include "Muscle.h"
#include "Strap.h"

#include <QDialog>

namespace Ui {
class DialogStringOfPearlsBuilder;
}

class DialogStringOfPearlsBuilder : public QDialog
{
    Q_OBJECT

public:
    explicit DialogStringOfPearlsBuilder(QWidget *parent = nullptr);
    virtual ~DialogStringOfPearlsBuilder() Q_DECL_OVERRIDE;

    void lateInitialise();

    void setSimulation(GaitSym::Simulation *newSimulation);

    std::vector<std::unique_ptr<GaitSym::Body> > *bodyList();

    std::vector<std::unique_ptr<GaitSym::Marker> > *markerList();

    std::vector<std::unique_ptr<GaitSym::Geom> > *geomList();

    std::vector<std::unique_ptr<GaitSym::Muscle> > *muscleList();

    std::vector<std::unique_ptr<GaitSym::Strap> > *strapList();

public slots:
    virtual void accept() Q_DECL_OVERRIDE;
    virtual void reject() Q_DECL_OVERRIDE;
    void properties();
    void importPathFromMuscle();
    void spinBoxNumberOfPearlsChanged(const QString &text);

protected:
    // void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::DialogStringOfPearlsBuilder *ui;

    GaitSym::Simulation *m_simulation = nullptr;

    QMap<QString, SettingsItem> m_properties;

    std::vector<std::unique_ptr<GaitSym::Body>> m_bodyList;
    std::vector<std::unique_ptr<GaitSym::Marker>> m_markerList;
    std::vector<std::unique_ptr<GaitSym::Geom>> m_geomList;
    std::vector<std::unique_ptr<GaitSym::Muscle>> m_muscleList;
    std::vector<std::unique_ptr<GaitSym::Strap>> m_strapList;

    void createBodies();
    void createGeoms();
    void createMuscles();

    void updateActivation();

};

#endif // DIALOGSTRINGOFPEARLSBUILDER_H
