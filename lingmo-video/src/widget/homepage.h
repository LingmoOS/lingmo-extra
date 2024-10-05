#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

#define CenterLogoSize  QSize(500, 65)
#define MainButtonSize  QSize(118, 40)

class QLabel;
class QPushButton;

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

signals:
    void openFileClicked();
    void openDirClicked();

private:
    QWidget *m_background;

    QPushButton *m_btnOpenFile,
                *m_btnOpenDir,
                *m_btnLogo;

    QLabel *m_labLogoName;

private slots:
    void gSettingChanged(QString key);

protected:
    void resizeEvent(QResizeEvent *e) override;
};

#endif // HOMEPAGE_H
