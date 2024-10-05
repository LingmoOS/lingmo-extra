#ifndef MEDIAINFODIALOG_H
#define MEDIAINFODIALOG_H

#include <QDialog>
#include "core/mpvtypes.h"

#define IconFixedSize QSize(24,24)

namespace Ui {
class MediaInfoDialog;
}

class MediaInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MediaInfoDialog(QWidget *parent = nullptr);
    ~MediaInfoDialog();

    void setData(QString data);
    void setTitle(QString s)    {m_title = s;}
    void setType(QString s)     {m_type = s;}
    void setSize(QString s)     {m_size = s;}
    void setDuration(QString s) {m_duration = s;}
    void setPath(QString s)     {m_filePath = s;}
    void updateDate();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MediaInfoDialog *ui;
    QString m_title,
            m_type,
            m_size,
            m_duration,
            m_filePath;

protected:
    void showEvent(QShowEvent *event) override;
};

#endif // MEDIAINFODIALOG_H
