#include "kchartview.h"
#include "../macro.h"

#include <QPainter>
#include <QPainterPath>

KChartView::KChartView(QWidget *parent)
    : QWidget(parent)
{
    m_dataMax = 100;
    setAttribute(Qt::WA_TranslucentBackground);
    m_colorBgLine = QColor("#78788050");
    initUI();
}

KChartView::~KChartView()
{
}

qreal KChartView::getCurData(int nItemIndex)
{
    if (nItemIndex >= 0 && nItemIndex < m_listChartAttr.size()) {
        if (m_listChartAttr[nItemIndex].listData.isEmpty()) {
            return 0;
        }
        return m_listChartAttr[nItemIndex].listData.at(m_listChartAttr[nItemIndex].listData.size()-1);
    }
    return 0;
}

qreal KChartView::getRealMaxData(qreal newData, int nItemIndex)
{
    qreal realMaxData = newData;
    if (nItemIndex < 0 || nItemIndex >= m_listChartAttr.size())
        return realMaxData;
    for (int n = 0; n < m_listChartAttr[nItemIndex].listData.size(); n++) {
        if (realMaxData < m_listChartAttr[nItemIndex].listData[n]) {
            realMaxData = m_listChartAttr[nItemIndex].listData[n];
        }
    }
    return realMaxData;
}

void KChartView::setMaxData(qreal qMaxData)
{
    if (qMaxData<=0)
        return;
    m_dataMax = qMaxData;
}

void KChartView::addItemAttr(KChartAttr chartAttr)
{
    QLineSeries* upLineSeries = new QLineSeries(this);
    QLineSeries* lowLineSeries = new QLineSeries(this);
    QAreaSeries* areaSeries = new QAreaSeries(this);
    areaSeries->setPen(chartAttr.colorForeground);
    QColor desColor = chartAttr.colorForeground;
    desColor.setAlpha(chartAttr.colorAlpha);
    areaSeries->setBrush(QBrush(desColor));
    areaSeries->setUpperSeries(upLineSeries);
    areaSeries->setLowerSeries(lowLineSeries);
    m_chart->addSeries(areaSeries);

    areaSeries->attachAxis(m_valueAxisX);
    areaSeries->attachAxis(m_valueAxisY);
    m_listChartAttr.append(chartAttr);
    m_listAreaSeries.append(areaSeries);
    m_listLowLineSeries.append(lowLineSeries);
    m_listUpLineSeries.append(upLineSeries);
}

void KChartView::onUpdateData(qreal fData, int nItemIndex)
{
    if (nItemIndex < 0 || nItemIndex >= m_listChartAttr.size())
        return;
    if (fData > m_dataMax) {
        fData = m_dataMax;
    }
    m_listChartAttr[nItemIndex].listData.append(fData);
    while (m_listChartAttr[nItemIndex].listData.size() > TEMP_CHARTSDATA_MAX) {
        m_listChartAttr[nItemIndex].listData.pop_front();
    }
    QList<QPointF> listUp;
    QList<QPointF> listLow;
    for (int n = 0; n < m_listChartAttr[nItemIndex].listData.size(); n++) {
        QPointF pointUp;
        QPointF pointLow;
        pointUp.setX(n);
        pointUp.setY(m_listChartAttr[nItemIndex].listData[n]*100.0/m_dataMax);
        listUp.append(pointUp);
        pointLow.setX(n);
        pointLow.setY(0);
        listLow.append(pointLow);
    }

    m_listUpLineSeries[nItemIndex]->clear();
    m_listUpLineSeries[nItemIndex]->replace(listUp);

    m_listLowLineSeries[nItemIndex]->clear();
    m_listLowLineSeries[nItemIndex]->replace(listLow);

    m_listAreaSeries[nItemIndex]->setUpperSeries(m_listUpLineSeries[nItemIndex]);
    m_listAreaSeries[nItemIndex]->setLowerSeries(m_listLowLineSeries[nItemIndex]);
}

void KChartView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();
    QPainterPath framePath;
    QRect desRect = rect();
    desRect.adjust(1,1,-1,-1);
    framePath.addRoundedRect(desRect,4,4);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(m_colorBgLine));
    painter.drawPath(framePath);
    int distance = desRect.height()/4;
    painter.drawLine(desRect.x(), desRect.y() + distance, desRect.right(), desRect.y() + distance);
    painter.drawLine(desRect.x(), desRect.y() + distance * 2, desRect.right(), desRect.y() + distance * 2);
    painter.drawLine(desRect.x(), desRect.y() + distance * 3, desRect.right(), desRect.y() + distance * 3);
    painter.restore();
    QWidget::paintEvent(event);
}

void KChartView::initUI()
{
    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setContentsMargins(3,0,0,0);
    m_mainLayout->setSpacing(0);
    m_chartLayout = new QHBoxLayout();
    m_chartLayout->setContentsMargins(0,0,0,0);
    m_chartLayout->setMargin(0);
    m_chartLayout->setSpacing(0);

    m_chart = new QChart();
    m_chart->legend()->hide();
    m_chart->setMargins(QMargins(-8,-8,-8,-8));
    m_chartView = new QChartView(this);
    m_chartView->setChart(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_chartView->setContentsMargins(0,0,0,0);
    m_chartLayout->addWidget(m_chartView);

    m_chart->setBackgroundVisible(false);
    m_chartView->setStyleSheet("background: transparent");

    m_valueAxisX = new QValueAxis(this);
    m_valueAxisY = new QValueAxis(this);
    m_valueAxisX->setRange(0, TEMP_CHARTSDATA_MAX);
    m_valueAxisX->setVisible(false);
   // m_valueAxisX->setReverse();
    m_valueAxisY->setRange(0, 100.0);
    m_valueAxisY->setVisible(false);

    m_chart->addAxis(m_valueAxisX, Qt::AlignBottom);
    m_chart->addAxis(m_valueAxisY, Qt::AlignLeft);

    m_mainLayout->addLayout(m_chartLayout);
    this->setLayout(m_mainLayout);
}
