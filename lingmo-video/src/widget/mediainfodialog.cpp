#include "mediainfodialog.h"
#include "ui_mediainfodialog.h"

#include <QDebug>
#include <QScrollBar>
#include <QMouseEvent>

#include <KF5/KWindowSystem/kwindoweffects.h>

#include <lingmo-log4qt.h>
#include "global/global.h"
#include "global/globalsignal.h"

MediaInfoDialog::MediaInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MediaInfoDialog)
{
    ui->setupUi(this);
    setFixedSize(400, 330);

    ui->btn_icon->setFixedSize(QSize(26,26));
    ui->btn_icon->setIconSize(QSize(26,26));
    ui->btn_icon->setIcon(QIcon::fromTheme("lingmo-video"));
    ui->btn_icon->setStyleSheet("QPushButton{border:0px;background:transparent;}"
                                "QPushButton::hover{border:0px;background:transparent;}"
                                "QPushButton::pressed{border:0px;background:transparent;}");

    ui->textBrowser->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
    // ui->textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    installEventFilter(this);
}

MediaInfoDialog::~MediaInfoDialog()
{
    delete ui;
}

void MediaInfoDialog::setData(QString data)
{
    ui->textBrowser->setText(data);
}

void MediaInfoDialog::updateDate()
{
    QString data = QString("<body style=\"line-height:28px;\"><font size=\"4\">%1</font><br>").arg(tr("media info"));
    data.append(tr("title:").append(m_title).append("<br>"));
    data.append(tr("type:").append(m_type).append("<br>"));
    data.append(tr("size:").append(m_size).append("<br>"));
    data.append(tr("duration:").append(m_duration).append("<br>"));
    data.append(tr("path:").append(m_filePath).append("</body>"));

    ui->textBrowser->setText(data);
}

void MediaInfoDialog::on_pushButton_clicked()
{
    hide();
}

void MediaInfoDialog::showEvent(QShowEvent *event)
{
    // 背景模糊
    KWindowEffects::enableBlurBehind(winId(), true);
    return QDialog::showEvent(event);
}
