#include "sidebar.h"
#include "global/interactiveqml.h"
#include "global/variable.h"
SideBar::SideBar(QWidget *parent) : QWidget(parent)
{

    m_sideList = new SideList(this);
    m_sideList->setWrapping(false);
    m_sideList->setDragEnabled(false);                              //设置不可拖动
    m_sideList->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可输入文字
    m_sideList->setViewMode(QListView::IconMode);
    m_sideList->setMouseTracking(true);
    m_sideList->setFrameShape(QListView::NoFrame);
    //毛玻璃
    this->setProperty("useSystemStyleBlur", true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    //设置鼠标穿透-解决鼠标移动到顶栏位置，顶栏不弹出的问题
    m_sideList->show();
    this->showItem();
    //响应拖拽事件
    m_sideList->setAcceptDrops(false);
    this->setFocusPolicy(Qt::NoFocus);
    this->initConnect();
    //    this->setAutoFillBackground(true);
    //    this->setBackgroundRole(QPalette::Base);

    //绘制阴影
    //    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    //    effect->setOffset(0, 0); //设置向哪个方向产生阴影效果(dx,dy)，特别地，(0,0)代表向四周发散
    //    effect->setColor(ALBUM_COLOR);       //设置阴影颜色，也可以setColor(QColor(220,220,220))
    //    effect->setBlurRadius(ALBUM_RADIUS); //设定阴影的模糊半径，数值越大越模糊
    //    this->setGraphicsEffect(effect);
    //    this->setContentsMargins(1, 1, 1, 1);
}

void SideBar::showItem()
{

    m_sideList->setModel(Interaction::getInstance()->getAlbumModel());
}

void SideBar::setItemNum(double acturalSize)
{
    if (acturalSize <= 0) {
        return;
    }
    if (SIDEITEM_SIZE.height() <= 0) {
        return;
    }
    if (m_imageNum <= 0) {
        return;
    }
    int itemNum = 0;
    if (Variable::g_mainlineVersion) {
        itemNum = acturalSize / (SIDEITEM_SIZE.height());
    } else {
        itemNum = acturalSize / (SIDEITEM_TABLET_SIZE.width());
    }

    if (itemNum >= m_imageNum) {
        Variable::LOADIMAGE_NUM = m_imageNum;
        Variable::g_needChangeAlbumPos = true;
        if (Variable::g_mainlineVersion) {
            this->resize(SIDEBAR_SIZE.width(), (56 + 6 + 8) * m_imageNum + 10);
            m_sideList->resize(SIDEBAR_SIZE.width(), (56 + 6 + 8) * m_imageNum + 10);
            m_sideList->move(0, 0);
        } else {
            this->resize(acturalSize, SIDEBAR_TABLET_SIZE.height());
            m_sideList->resize(SIDEITEM_TABLET_SIZE.width() * m_imageNum, SIDEBAR_TABLET_SIZE.height() - 8);
            m_sideList->move(this->width() / 2 - m_sideList->width() / 2, 8);
        }

    } else {
        Variable::LOADIMAGE_NUM = itemNum;
        Variable::g_needChangeAlbumPos = false;
        if (Variable::g_mainlineVersion) {
            this->resize(SIDEBAR_SIZE.width(), acturalSize);
            m_sideList->resize(SIDEBAR_SIZE.width(), acturalSize);
            m_sideList->move(0, 0);
        } else {
            this->resize(acturalSize, SIDEBAR_TABLET_SIZE.height());
            m_sideList->resize(acturalSize, SIDEBAR_TABLET_SIZE.height() - 8);
            m_sideList->move(0, 8);
        }
    }

    dealScrollbarMove(1);
}

void SideBar::setSideBarQss(QString themeStyle)
{
    QString border = "4px";
    QString marginTop = "";
    if (Variable::g_mainlineVersion) {
        border = "4px";
        marginTop = "";
    } else {
        border = "0px";
        marginTop = "margin-top:16px;/*margin-bottom:0px;*/";
        return;
    }
    if ("lingmo-dark" == themeStyle || "lingmo-black" == themeStyle) {
        this->setStyleSheet("QListView{border:1px "
                            ";border-top-left-radius:0px;border-top-right-radius:"
                            + border
                            + ";border-bottom-left-radius:"
                              "0px;border-bottom-right-radius:"
                            + border
                            + ";outline:none;/*background:rgba(63, 69, 77, 1)*/background:red}"
                              "QListView::item{width:96px;height:96px;border:2px solid rgba(13, 135, 255,0);"
                            + marginTop + "background:rgba(0, 0, 0, 0.4);border-radius:" + border
                            + ";}"
                              "QListView::item:selected{"
                            + marginTop
                            + "border:2px solid rgba(13, 135, 255, 1);background:rgba(0, "
                              "0, 0, 0.4);border-radius:"
                            + border
                            + ";}"
                              "QListView::item:hover{background:rgba(0, 0, 0, 0.4);border-radius:"
                            + border + ";}");
    } else {
        this->setStyleSheet("QListView{border:1px "
                            ";border-top-left-radius:0px;border-top-right-radius:"
                            + border
                            + ";border-bottom-left-radius:0px;border-bottom-right-"
                              "radius:"
                            + border
                            + ";outline:none;background:rgba(227, 235, 239, 1)/*background:red*/}"
                              "QListView::item{width:96px;height:96px;border:2px solid rgba(13, 135, 255,0);"
                            + marginTop
                            + "background:rgba(255, 255, 255, 0.5)/*background:yellow*/;border-radius:" + border
                            + ";}"
                              "QListView::item:selected{"
                            + marginTop
                            + "border:2px solid rgba(13, 135, 255, "
                              "0.86);background:rgba(255, "
                              "255, 255, "
                              "0.9);border-radius:"
                            + border
                            + ";}"
                              "QListView::item:hover{background:rgba(255, 255, 255, 0.9);border-radius:"
                            + border + ";}");
    }
}

void SideBar::setScrollBarStyle()
{
    if (Variable::g_mainlineVersion) {
        m_sideList->setFlow(QListView::TopToBottom);
        m_sideList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);           //设置无水平滑动条
        m_sideList->setIconSize(SIDEITEM_SIZE);                                     //
        m_sideList->verticalScrollBar()->setProperty("drawScrollBarGroove", false); //设置滑动条无滑槽
    } else {
        m_sideList->setFlow(QListView::LeftToRight);
        m_sideList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);               //设置无垂直滑动条
        m_sideList->setIconSize(SIDEITEM_TABLET_SIZE);                                //
        m_sideList->horizontalScrollBar()->setProperty("drawScrollBarGroove", false); //设置滑动条无滑槽
    }
}

void SideBar::getSelect(int type)
{
    m_imageNum = type;
    if (Variable::g_mainlineVersion) {
        setItemNum(Variable::g_acturalWidHeight);
    } else {
        setItemNum(Variable::g_acturalWidWidth);
    }
    Q_EMIT sizeChange();
}

void SideBar::openEmptyFile(bool changeHigh)
{
    if (changeHigh == true) {
        setCurrIndex(m_modelIndexOld);
    }
}

void SideBar::initConnect()
{
    //    connect(m_sideList, &kdk::KListView::clicked, Interaction::getInstance(), &Interaction::changeImageFromClick);
    connect(Interaction::getInstance(), &Interaction::changeAlbumHighLight, this, &SideBar::setCurrIndex);
    connect(Interaction::getInstance(), &Interaction::updateSideLength, this, &SideBar::getSelect);
    //    connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &SideBar::dealScrollbarMove);
    connect(m_sideList->horizontalScrollBar(), &QScrollBar::valueChanged, this, &SideBar::dealScrollbarMove);
}

void SideBar::setCurrIndex(QModelIndex modelIndex)
{
    //当为第一张图时，回到顶部
    if (modelIndex.row() == 1) {
        m_sideList->horizontalScrollBar()->setValue(0);
    }
    //存上一个，为了点击加号，但不打开时，能切换到正确的显示
    if (modelIndex.row() != 0) {
        Q_EMIT changeImage(true);
        m_modelIndexOld = modelIndex;
    } else {
        Q_EMIT changeImage(false);
    }
    m_sideList->setCurrentIndex(modelIndex);
}
void SideBar::dealScrollbarMove(int moveValue)
{
    if (Variable::g_mainlineVersion) {
        QPoint centerPoint = m_sideList->viewport()->contentsRect().center();
        if (m_sideList->indexAt(m_sideList->viewport()->contentsRect().center()).row() == -1) {
            if (m_sideList->indexAt(QPoint(centerPoint.x(), centerPoint.y() + 8)).row() == -1) {
                Interaction::getInstance()->reloadImage(
                    m_sideList->indexAt(QPoint(centerPoint.x(), centerPoint.y() - 8)));
            } else {
                Interaction::getInstance()->reloadImage(
                    m_sideList->indexAt(QPoint(centerPoint.x(), centerPoint.y() + 8)));
            }
        } else {
            Interaction::getInstance()->reloadImage(m_sideList->indexAt(centerPoint));
        }
    } else {
        QPoint centerPoint = m_sideList->viewport()->contentsRect().center();
        Interaction::getInstance()->reloadImage(m_sideList->indexAt(centerPoint));
    }
}


// void SideBar::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing); // 反锯齿;
//                                                   //设置背景颜色
//    QColor color = this->palette().color(QPalette::Dark);
//    QPalette pal(this->palette());
//    pal.setColor(QPalette::Base, QColor(color));
//    this->setPalette(pal);
//    //    QBrush brush = QBrush(color);
//    //    painter.setBrush(brush);
//    //    painter.setOpacity(0.1);
//    painter.setPen(Qt::transparent);
//    QStyleOption opt;
//    opt.init(this);
//    painter.drawRoundedRect(opt.rect, 18, 18);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
//    QWidget::paintEvent(event);
//}

void SideBar::paintEvent(QPaintEvent *event)
{
    //使用主题给的代码适配毛玻璃效果
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    QColor color = qApp->palette().color(QPalette::Base);
    color.setAlphaF(0.95);
    QPalette pal(this->palette());
    pal.setColor(QPalette::Window, QColor(color));
    this->setPalette(pal);
    QBrush brush = QBrush(color);
    p.setBrush(brush);
    p.drawRoundedRect(opt.rect, 0, 0);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
