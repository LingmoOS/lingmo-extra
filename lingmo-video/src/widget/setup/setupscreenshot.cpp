#include "setupscreenshot.h"

#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QStandardPaths>
#include <QFileSystemWatcher>

#include "global/global.h"
#include "../checkbox.h"

using namespace Global;

SetupScreenshot::SetupScreenshot(QWidget *parent) :
    ThemeWidget(parent)
{
    initLayout();
    m_format_index = 0;

    QFont f("Noto Sans CJK SC Regular");
    f.setPixelSize(14);
    m_save2clipRadioButton->setFont(f);
    m_save2fileRadioButton->setFont(f);
    m_label1->setFont(f);
    m_label2->setFont(f);
    m_lineEdit->setFont(f);
    m_comboBox->setFont(f);
    m_browserButton->setFont(f);
    m_checkBox->setFont(f);

    m_lineEdit->setReadOnly(true);

    initConnect();

    // 保存到剪切板功能和当前尺寸截图功能未实现
    m_save2clipRadioButton->hide();
    m_checkBox->hide();

    connectThemeSetting();
}

SetupScreenshot::~SetupScreenshot()
{
}

void SetupScreenshot::initData()
{
    if(g_config->screenShotSaveToClip.first)
        m_save2clipRadioButton->setChecked(true);
    else
        m_save2fileRadioButton->setChecked(true);

    if(g_config->screenShotPath.first.length() == 0)
    {
        g_config->screenShotPath.second = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        g_config->flushChange();
    }

    // 设置支持的截图格式
    QStringList format;
    format << "jpg" << "png" << "webp";
    for(int i=0; i<format.size(); i++)
        if(format.at(i) == g_config->screenShotFormat.first)
            m_format_index = i;

    m_comboBox->clear();
    m_comboBox->addItems(format);
    m_comboBox->setCurrentIndex(m_format_index);

    m_checkBox->setChecked(g_config->screenShotCurrentSize.first);
}

void SetupScreenshot::setBlackTheme()
{
    m_save2clipRadioButton->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_save2fileRadioButton->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_label1->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_label2->setStyleSheet(QString("color:rgb(249,249,249);"));
//    m_lineEdit->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
//    m_comboBox->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
    m_browserButton->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
    m_checkBox->setStyleSheet(QString("color:rgb(249,249,249);"));
}

void SetupScreenshot::setDefaultTheme()
{
    m_save2clipRadioButton->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_save2fileRadioButton->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_label1->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_label2->setStyleSheet(QString("color:rgb(38,38,38);"));
//    m_lineEdit->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
//    m_comboBox->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
    m_browserButton->setStyleSheet("QPushButton{background-color:rgb(224,224,224);color:rgb(38,38,38);}"
                                  "QPushButton:hover{color:rgb(255,255,255);}");
    m_checkBox->setStyleSheet(QString("color:rgb(38,38,38);"));
}

void SetupScreenshot::setWidgetFont(QString family, int size)
{
    QFont f(family);
    f.setPointSize(size);

    m_save2clipRadioButton->setFont(f);
    m_save2fileRadioButton->setFont(f);
    m_label1->setFont(f);
    m_label2->setFont(f);
    m_lineEdit->setFont(f);
    m_comboBox->setFont(f);
    m_browserButton->setFont(f);
    m_checkBox->setFont(f);
}

void SetupScreenshot::initLayout()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(30, 10, 8, 8);
    lay->setSpacing(12);

    m_save2clipRadioButton = new QRadioButton;
    m_save2clipRadioButton->setText(tr("Only save to clipboard"));
    lay->addWidget(m_save2clipRadioButton);

    m_save2fileRadioButton = new QRadioButton;
    m_save2fileRadioButton->setText(tr("Save to file"));
    lay->addWidget(m_save2fileRadioButton);

    {
        m_hboxLayout1 = new QHBoxLayout;
        m_hboxLayout1->setContentsMargins(0, 0, 16, 0);
        m_hboxLayout1->setSpacing(8);

        m_label1 = new QLabel(tr("save path"));
        m_hboxLayout1->addWidget(m_label1);

        m_lineEdit = new QLineEdit;
        m_hboxLayout1->addWidget(m_lineEdit);

        m_browserButton = new QPushButton(tr("browser"));
        m_hboxLayout1->addWidget(m_browserButton);

        lay->addLayout(m_hboxLayout1);
    }

    {
        m_hboxLayout2 = new QHBoxLayout;
        m_hboxLayout2->setContentsMargins(0, 0, 15, 0);
        m_hboxLayout2->setSpacing(8);

        m_label2 = new QLabel(tr("save type"));
        m_hboxLayout2->addWidget(m_label2);

        m_comboBox = new QComboBox;
        m_hboxLayout2->addWidget(m_comboBox);

        m_hboxLayout2->setStretchFactor(m_comboBox, 1);

        lay->addLayout(m_hboxLayout2);
    }

    m_checkBox = new CheckBox;
    m_checkBox->setText(tr("Screenshot according to the current screen size"));

    lay->addWidget(m_checkBox);

    lay->addStretch();
}

void SetupScreenshot::initConnect()
{
    connect(m_save2clipRadioButton, &QRadioButton::toggled, [&](bool checked){g_config->screenShotSaveToClip.second = checked;});
    connect(m_browserButton, &QPushButton::clicked, [&](bool checked){
        // 打开文件夹
        QString url;

        {
            QFileDialog fd(parentWidget());
            fd.setModal(true);
            QList<QUrl> list = fd.sidebarUrls();
            int sidebarNum = 8;
            QString home = QDir::homePath().section("/", -1, -1);
            QString mnt = "/media/" + home + "/";
            QDir mntDir(mnt);
            mntDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
            QFileInfoList filist = mntDir.entryInfoList();
            QList<QUrl> mntUrlList;
            for(int i=0; i < sidebarNum && i < filist.size(); i++) {
                QFileInfo fi = filist.at(i);
                //华为990、9a0需要屏蔽最小系统挂载的目录
                if (fi.fileName() == "2691-6AB8")
                     continue;
                mntUrlList << QUrl("file://" + fi.filePath());
            }
            QFileSystemWatcher fsw(&fd);
            fsw.addPath("/media/" + home + "/");
            connect(&fsw, &QFileSystemWatcher::directoryChanged, &fd, [=, &sidebarNum, &mntUrlList, &list, &fd](const QString path){
                QDir wmnDir(path);
                wmnDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
                QFileInfoList wfilist = wmnDir.entryInfoList();
                mntUrlList.clear();
                for(int i=0; i < sidebarNum && i < wfilist.size(); ++i) {
                    QFileInfo fi = wfilist.at(i);
                    //华为990、9a0需要屏蔽最小系统挂载的目录
                    if (fi.fileName() == "2691-6AB8")
                         continue;
                    mntUrlList << QUrl("file://" + fi.filePath());
                }
                fd.setSidebarUrls(list + mntUrlList);
                fd.update();
            });
            connect(&fd, &QFileDialog::finished, &fd, [=, &list, &fd](){
                fd.setSidebarUrls(list);
            });
            fd.setSidebarUrls(list + mntUrlList);
            fd.setDirectory(QDir(m_lineEdit->text()));
            fd.setWindowTitle(tr("Video Player Choose a directory"));
            fd.setFileMode(QFileDialog::DirectoryOnly);

            g_shortcut->makeAllInvalid();
            if(fd.exec() == QFileDialog::Accepted)
            {
                url = fd.selectedUrls().at(0).toString();
                if(url.startsWith("file"))
                    url.remove(0, 7);
            }
            g_shortcut->makeAllValid();
        }

        if(url.length() > 0) {
            g_config->screenShotPath.second = url;
        }
    });
    connect(m_comboBox, &QComboBox::currentTextChanged, [&](QString format){g_config->screenShotFormat.second = format;});
    connect(m_checkBox, &QCheckBox::toggled, [&](bool checked){g_config->screenShotCurrentSize.second = checked;});
}

void SetupScreenshot::paintEvent(QPaintEvent *e)
{
    QFontMetrics fontWidth(m_lineEdit->font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(g_config->screenShotPath.second, Qt::ElideMiddle, m_lineEdit->width() - m_lineEdit->font().pointSizeF() * 1.5);
    m_lineEdit->setText(show_name);
    m_lineEdit->setToolTip(show_name == g_config->screenShotPath.second ?
                                "" : g_config->screenShotPath.second);
    return QWidget::paintEvent(e);
}
