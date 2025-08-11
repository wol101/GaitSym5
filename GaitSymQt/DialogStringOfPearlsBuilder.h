#ifndef DIALOGSTRINGOFPEARLSBUILDER_H
#define DIALOGSTRINGOFPEARLSBUILDER_H

#include "Simulation.h"
#include "Preferences.h"

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

    void updateActivation();

};

#endif // DIALOGSTRINGOFPEARLSBUILDER_H
