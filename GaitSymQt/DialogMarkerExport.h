#ifndef DIALOGMARKEREXPORT_H
#define DIALOGMARKEREXPORT_H

#include <QDialog>

#include <memory>

namespace GaitSym
{
class Simulation;
class Marker;
}

namespace Ui { class DialogMarkerExport; }

class DialogMarkerExport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMarkerExport(QWidget *parent = nullptr);
    ~DialogMarkerExport() Q_DECL_OVERRIDE;

    int ExportMarkers();

    void setSimulation(GaitSym::Simulation *simulation);

    std::vector<std::unique_ptr<GaitSym::Marker> > *markerList() const;

public slots:
    virtual void reject() Q_DECL_OVERRIDE;
    void processButtonClicked();
    void fileNameTextChanged(const QString &text);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::DialogMarkerExport *ui;

    GaitSym::Simulation *m_simulation = nullptr;

    void SetUIElementsFromPreferences();
    void SaveUIElementsToPreferences();

    void EnableWidgets();
};

#endif // DIALOGMARKEREXPORT_H
