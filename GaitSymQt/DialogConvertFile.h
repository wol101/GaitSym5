#ifndef DIALOGCONVERTFILE_H
#define DIALOGCONVERTFILE_H

#include <QDialog>

namespace Ui {
class DialogConvertFile;
}

class DialogConvertFile : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConvertFile(QWidget *parent = nullptr);
    ~DialogConvertFile();

public slots:
    void pushButtonInputFileClicked();
    void pushButtonOutputFileClicked();
    void pushButtonConvertClicked();
    void lineEditTextChanged(const QString &text);

    static void layoutSpacing(QWidget *container);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::DialogConvertFile *ui;

    void enableWidgets();
    void readSettings();
    void writeSettings();
    void doConversion();
    void log(const QString &text);
};

#endif // DIALOGCONVERTFILE_H
