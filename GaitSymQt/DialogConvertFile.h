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
    virtual void accept() Q_DECL_OVERRIDE;
    virtual void reject() Q_DECL_OVERRIDE;

    void pushButtonInputFileClicked();
    void pushButtonOutputFileClicked();
    void pushButtonConvertClicked();
    void lineEditTextChanged(const QString &text);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::DialogConvertFile *ui;

    void enableWidgets();
    void readSettings();
    void writeSettings();
    void doConversion();
};

#endif // DIALOGCONVERTFILE_H
