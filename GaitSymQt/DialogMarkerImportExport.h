#ifndef DIALOGMARKERIMPORTEXPORT_H
#define DIALOGMARKERIMPORTEXPORT_H

#include <QDialog>

#include <memory>

namespace GaitSym
{
class Simulation;
class Marker;
}

namespace Ui { class DialogMarkerImportExport; }

class DialogMarkerImportExport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMarkerImportExport(QWidget *parent = nullptr);
    ~DialogMarkerImportExport() Q_DECL_OVERRIDE;

    int ImportMarkers();
    int ExportMarkers();

    void setSimulation(GaitSym::Simulation *simulation);

    void setMarkerList(std::vector<std::unique_ptr<GaitSym::Marker> > *markerList);

    std::vector<std::unique_ptr<GaitSym::Marker> > *markerList() const;

    void setAllowImport(bool newAllowImport);

public slots:
    virtual void accept() Q_DECL_OVERRIDE;
    virtual void reject() Q_DECL_OVERRIDE;
    void radioButtonExportClicked();
    void radioButtonImportClicked();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::DialogMarkerImportExport *ui;

    GaitSym::Simulation *m_simulation = nullptr;
    std::vector<std::unique_ptr<GaitSym::Marker>> *m_markerList = nullptr;
    bool m_allowImport = false;

    void SetUIElementsFromPreferences();
    void SaveUIElementsToPreferences();
};

#endif // DIALOGMARKERIMPORTEXPORT_H
