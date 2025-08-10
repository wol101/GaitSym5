#ifndef DIALOGSTRINGOFPEARLSBUILDER_H
#define DIALOGSTRINGOFPEARLSBUILDER_H

#include <QDialog>

namespace Ui {
class DialogStringOfPearlsBuilder;
}

class DialogStringOfPearlsBuilder : public QDialog
{
    Q_OBJECT

public:
    explicit DialogStringOfPearlsBuilder(QWidget *parent = nullptr);
    ~DialogStringOfPearlsBuilder();

private:
    Ui::DialogStringOfPearlsBuilder *ui;
};

#endif // DIALOGSTRINGOFPEARLSBUILDER_H
