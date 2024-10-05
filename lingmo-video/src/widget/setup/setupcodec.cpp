#include "setupcodec.h"

#include "global/global.h"

#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QIntValidator>

using namespace Global;

SetupCodec::SetupCodec(QWidget *parent) :
    ThemeWidget(parent)
{
    initLayout();
    QIntValidator *ivd = new QIntValidator;
    ivd->setRange(1, 32);
    m_threadEdit->setValidator(ivd);
    m_threadEdit->setFixedHeight(30);

    initData();
    initConnect();
    connectThemeSetting();
}

SetupCodec::~SetupCodec()
{
    if (m_threadEdit->validator()) {
        delete m_threadEdit->validator();
    }
}

void SetupCodec::initData()
{
    QStringList items;
    items << tr("default") << tr("no") << "vdpau" << "vdpau-copy" << "vaapi" << "vaapi-copy";
    m_comboBoxDecoder->addItems(items);
    m_comboBoxDecoder->setCurrentText(tr(g_settings->value("General/video_decoder").toString().toStdString().c_str()));

    m_label1->setText(tr("Video output"));
    m_comboBoxOutput->addItem(tr("auto"));
    m_comboBoxOutput->addItem("x11");
    m_comboBoxOutput->addItem("xv");
    m_comboBoxOutput->addItem("vdpau");
    m_comboBoxOutput->addItem("vaapi");
    m_comboBoxOutput->addItem("gpu");
    m_comboBoxOutput->setCurrentText(g_settings->value("General/video_output").toString());

    m_label2->setText(tr("Video decoder"));

    m_label3->setText(tr("Decode threads"));
    int threads = g_settings->value("General/video_decode_threads").toInt();
    m_threadEdit->setText(QString::number(threads > 0 ? threads : 4));
}

void SetupCodec::initConnect()
{
    connect(m_comboBoxDecoder, &QComboBox::currentTextChanged, this, [this](QString decoder) {
        g_config->videoDecoder.second = decoder;
    });
    connect(m_comboBoxOutput, &QComboBox::currentTextChanged, this, [this](QString decoder) {
        g_config->videoOutput.second = decoder;
    });
    connect(m_threadEdit, &QLineEdit::textChanged, this, [this](QString text){
        g_config->videoDecodeThreads.second = text.toInt();
    });
}

void SetupCodec::setBlackTheme()
{
    m_label1->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_label2->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_label3->setStyleSheet(QString("color:rgb(249,249,249);"));

//    m_comboBoxOutput->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
//    m_comboBoxDecoder->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
//    m_threadEdit->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
//    ui->cb_ADecoder->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
}

void SetupCodec::setDefaultTheme()
{
    m_label1->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_label2->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_label3->setStyleSheet(QString("color:rgb(38,38,38);"));

//    m_comboBoxOutput->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
//    m_comboBoxDecoder->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
//    m_threadEdit->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
    //    ui->cb_ADecoder->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
}

void SetupCodec::setWidgetFont(QString family, int size)
{
    QFont f(family);
    f.setPointSize(size + 2);
    m_label1->setFont(f);
    m_label2->setFont(f);
    m_label3->setFont(f);

    f.setPointSize(size);
    m_comboBoxDecoder->setFont(f);
    m_comboBoxOutput->setFont(f);
    m_threadEdit->setFont(f);
}

void SetupCodec::initLayout()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(30, 10, 24, 8);
    lay->setSpacing(12);

    m_label1 = new QLabel;
    lay->addWidget(m_label1);

    m_comboBoxOutput = new QComboBox;
    lay->addWidget(m_comboBoxOutput);

    m_label1->hide();
    m_comboBoxOutput->hide();

    if (g_config->videoOutputType() == GlobalConfig::VO_WID) {
        m_label1->show();
        m_comboBoxOutput->show();
    }

    m_label2 = new QLabel;
    lay->addWidget(m_label2);

    m_comboBoxDecoder = new QComboBox;
    lay->addWidget(m_comboBoxDecoder);

    m_label3 = new QLabel;
    lay->addWidget(m_label3);

    m_threadEdit = new QLineEdit;
    lay->addWidget(m_threadEdit);

    lay->addStretch();
}
