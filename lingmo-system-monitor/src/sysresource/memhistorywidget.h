#ifndef MEMHISTORYWIDGET_H
#define MEMHISTORYWIDGET_H

#include <QWidget>
#include <qgsettings.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>

#include "../controls/kchartview.h"
#include "../controls/kprogressbar.h"
#include "../controls/klabel.h"

class MemHistoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MemHistoryWidget(QWidget *parent = nullptr);
    virtual ~MemHistoryWidget();

    QString getMemOccupancy();

public slots:
    void onUpdateMemData(qreal memUsed, qreal memTotal);
    void onUpdateSwapData(qreal swapUsed, qreal swapTotal);
signals:

protected:
    void paintEvent(QPaintEvent *event);

private:
    void initUI();
    void initConnections();
    void initStyleTheme();
    void onThemeFontChange(float fFontSize);
    void onThemeStyleChange(QString strStyleName);

private:
    KChartView *m_chartView = nullptr;
    KProgressBar *m_progressBarMem = nullptr;
    KProgressBar *m_progressBarSwap = nullptr;
    KLabel *m_labelMemTitle = nullptr;
    KLabel *m_labelMemValue = nullptr;
    KLabel *m_labelSwapTitle = nullptr;
    KLabel *m_labelSwapValue = nullptr;

    QVBoxLayout *m_mainLayout = nullptr;
    QHBoxLayout *m_memTitleLayout = nullptr;
    QHBoxLayout *m_memProgLayout = nullptr;
    QHBoxLayout *m_swapTitleLayout = nullptr;
    QHBoxLayout *m_swapProgLayout = nullptr;
    QHBoxLayout *m_chartLayout = nullptr;

    QColor m_colorBg;
    QGSettings *m_styleSettings = nullptr;
    float m_curFontSize;
};

#endif // CPUHISTORYWIDGET_H
