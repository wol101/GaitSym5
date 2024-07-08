#ifndef DIALOGJOINTS_H
#define DIALOGJOINTS_H

#include "Preferences.h"

#include <QDialog>

#include <memory>

namespace GaitSym
{
class Simulation;
class Joint;
}

namespace Ui { class DialogJoints; }

class DialogJoints : public QDialog
{
    Q_OBJECT

public:
    explicit DialogJoints(QWidget *parent = nullptr);
    virtual ~DialogJoints() Q_DECL_OVERRIDE;

    void lateInitialise();

    GaitSym::Simulation *simulation() const;
    void setSimulation(GaitSym::Simulation *simulation);

    void setInputJoint(GaitSym::Joint *inputJoint);

    std::unique_ptr<GaitSym::Joint> outputJoint();

public slots:
    virtual void accept() Q_DECL_OVERRIDE;
    virtual void reject() Q_DECL_OVERRIDE;
    void properties();
    void comboBoxChanged(int index);
    void lineEditChanged(const QString &text);
    void spinBoxChanged(int value);
    void checkBoxChanged(int index);
    void tabChanged(int index);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::DialogJoints *ui = nullptr;

    GaitSym::Simulation *m_simulation = nullptr;
    GaitSym::Joint *m_inputJoint = nullptr;
    std::unique_ptr<GaitSym::Joint> m_outputJoint;

    QMap<QString, SettingsItem> m_properties;

    void updateActivation();
};

#endif // DIALOGJOINTS_H
