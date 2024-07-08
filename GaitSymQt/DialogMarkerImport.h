#ifndef DIALOGMARKERIMPORT_H
#define DIALOGMARKERIMPORT_H

#include <QDialog>

#include <memory>
#include <set>

namespace GaitSym
{
class Simulation;
class Marker;
}

namespace Ui { class DialogMarkerImport; }

class DialogMarkerImport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMarkerImport(QWidget *parent = nullptr);
    ~DialogMarkerImport() Q_DECL_OVERRIDE;

    int ImportMarkers();

    void setSimulation(GaitSym::Simulation *simulation);

    void setMarkerList(std::vector<std::unique_ptr<GaitSym::Marker> > *markerList);

    std::vector<std::unique_ptr<GaitSym::Marker> > *markerList() const;

public slots:
    virtual void reject() Q_DECL_OVERRIDE;
    void applyButtonClicked();
    void processButtonClicked();
    void fileNameTextChanged(const QString &text);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::DialogMarkerImport *ui;

    GaitSym::Simulation *m_simulation = nullptr;
    std::vector<std::unique_ptr<GaitSym::Marker>> *m_markerList = nullptr;
    std::set<std::string> m_addedNames;

    void SetUIElementsFromPreferences();
    void SaveUIElementsToPreferences();

    void EnableWidgets();
};

#endif // DIALOGMARKERIMPORT_H
