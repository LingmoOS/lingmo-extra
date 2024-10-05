#include "urleditwidget.h"

#include "global/global.h"
#include <lingmostylehelper/lingmostylehelper.h>

using namespace Global;

URLEditWidget::URLEditWidget(QWidget *parent) :
    QDialog(parent),
    m_url("")
{
    initLayout();
    m_url = g_settings->value("General/url").toString();
    if (m_url.length() != 0)
        m_line_edit_url->setText(m_url);
    kdk::LingmoStyleHelper::self()->removeHeader(this);
}

URLEditWidget::~URLEditWidget()
{
    delete m_button_icon;
    m_button_icon = nullptr;
    delete m_label_title;
    m_label_title = nullptr;
    delete m_button_close;
    m_button_close = nullptr;
    delete m_layout_title;
    m_layout_title = nullptr;
    delete m_line_edit_url;
    m_line_edit_url = nullptr;
    delete m_button_cancel;
    m_button_cancel = nullptr;
    delete m_button_ok;
    m_button_ok = nullptr;
    delete m_layout_button;
    m_layout_button = nullptr;
}

void URLEditWidget::initLayout()
{
    setFixedSize(500, 130);

    m_layout_widget = new QVBoxLayout(this);
    m_layout_widget->setContentsMargins(4, 4, 4, 4);

    m_layout_title = new QHBoxLayout;
    m_layout_title->setSpacing(4);
    m_layout_widget->addLayout(m_layout_title);

    m_button_icon = new QPushButton;
    m_button_icon->setFlat(true);
    m_button_icon->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    m_button_icon->setIcon(QIcon::fromTheme("lingmo-video"));
    m_button_icon->setFixedSize(QSize(26, 26));
    m_button_icon->setIconSize(QSize(26, 26));
    m_layout_title->addWidget(m_button_icon);

    m_label_title = new QLabel;
    m_label_title->setText(tr("Open URL"));
    m_layout_title->addWidget(m_label_title);
    m_layout_title->setStretchFactor(m_label_title, 0);

    m_button_close = new QPushButton;
    m_button_close->setFixedSize(36, 36);
    m_button_close->setFlat(true);
    m_button_close->setIcon(QIcon::fromTheme("window-close-symbolic"));
    m_button_close->setProperty("isWindowButton", 0x2);
    m_button_close->setProperty("useIconHighlightEffect", 0x8);
    m_button_close->setToolTip(tr("Close"));
    connect(m_button_close, &QPushButton::clicked, [this](){close();});
    m_layout_title->addWidget(m_button_close);

    m_line_edit_url = new QLineEdit;
    m_line_edit_url->setPlaceholderText(tr("Please input url link"));
    m_layout_widget->addWidget(m_line_edit_url);

    m_layout_button = new QHBoxLayout;
    m_layout_button->addStretch();
    m_layout_widget->addLayout(m_layout_button);

    m_button_cancel = new QPushButton;
    m_button_cancel->setText(tr("Cancel"));
    m_button_cancel->setProperty("isWindowButton", 0x2);
    m_button_cancel->setProperty("useIconHighlightEffect", 0x8);
    connect(m_button_cancel, &QPushButton::clicked, [this](){m_url = "";reject();});
    m_layout_button->addWidget(m_button_cancel);

    m_button_ok = new QPushButton;
    m_button_ok->setText(tr("Ok"));
    connect(m_button_ok, &QPushButton::clicked, [this](){
        m_url = m_line_edit_url->text();
        g_settings->setValue("General/url", m_url);
        accept();
    });
    m_layout_button->addWidget(m_button_ok);
}
