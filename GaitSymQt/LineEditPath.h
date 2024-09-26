#ifndef LINEEDITPATH_H
#define LINEEDITPATH_H

#include <QLineEdit>
#include <QValidator>

class QFocusEvent;

class LineEditPath : public QLineEdit
{
    Q_OBJECT
public:
    LineEditPath(QWidget *parent = nullptr);

    enum PathType { FileForOpen, FileForSave, Folder };

    PathType pathType() const;
    void setPathType(const PathType &pathType);

    void setHighlighted(bool highlight);

    void setText(const QString &text); // we are overloading the setText function
    QString text() const; // we are overloading the setText function

    QValidator::State state() const;

public slots:
    void menuRequestPath(const QPoint &pos);
    void textChangedSlot(const QString &text);

signals:
    void focussed(bool hasFocus);

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

private:
    void generateLocalStyleSheet();

    PathType m_pathType;
    QString m_backgroundStyle;
    QString m_foregroundStyle;
    QValidator::State m_state = QValidator::Invalid;
};

#endif // LINEEDITPATH_H
