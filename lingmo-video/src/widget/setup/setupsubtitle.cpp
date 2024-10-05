#include "setupsubtitle.h"

#include <QDir>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontDatabase>
#include <QFileSystemWatcher>

#include "global/global.h"
#include "../checkbox.h"

using namespace Global;

SetupSubtitle::SetupSubtitle(QWidget *parent) :
    ThemeWidget(parent)
{
    initLayout();

    QFontDatabase fdb;
    for (QString it : fdb.families()) {
        if (!it.toLower().contains("ubuntu")) {
            m_familyComboBox->addItem(it);
        }
    }

    // 字体选择列表 先自己设置一些
    QStringList size_list;
    size_list << "26" << "32" << "42" << "52" << "64" << "76";
    m_sizeComboBox->addItems(size_list);

    initConnect();

    m_subPathEdit->setReadOnly(true);

    connectThemeSetting();
}

SetupSubtitle::~SetupSubtitle()
{
}

void SetupSubtitle::initData()
{
    m_checkbox_load_same->setChecked(g_config->loadSameNameSub.first);
    // 如果默认字幕路径为空的话设置为家目录
    if (g_config->subDir.first == "") {
        g_config->subDir.second = QDir::homePath();
        g_config->flushChange();
    }
    m_subPathEdit->setText(g_config->subDir.first);

    // 加载字体
    QString font_family = g_config->subFontFamily.first;
    int font_size = g_config->subFontSize.first;
    QFont f;
    if(font_family.length() > 0)
        m_familyComboBox->setCurrentText(font_family);
    else
        m_familyComboBox->setCurrentText(f.defaultFamily());

    if(font_size == 0)
        m_sizeComboBox->setCurrentText(QString("%1").arg(f.pointSize()));
    else
        m_sizeComboBox->setCurrentText(QString("%1").arg(font_size));
}

void SetupSubtitle::setBlackTheme()
{
    m_label_title_1->setStyleSheet(QString("color:rgb(255,255,255);"));
    m_title2Label->setStyleSheet(QString("color:rgb(255,255,255);"));

//    m_familyComboBox->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
//    m_sizeComboBox->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
//    m_subPathEdit->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
    m_browserButton->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
    m_checkbox_load_same->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_label_path->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_label2->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_label3->setStyleSheet(QString("color:rgb(249,249,249);"));
}

void SetupSubtitle::setDefaultTheme()
{
    m_label_title_1->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_title2Label->setStyleSheet(QString("color:rgb(38,38,38);"));

//    m_familyComboBox->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
//    m_sizeComboBox->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
//    m_subPathEdit->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
    m_browserButton->setStyleSheet("QPushButton{background-color:rgb(224,224,224);color:rgb(38,38,38);}"
                                  "QPushButton:hover{color:rgb(255,255,255);}");
    m_checkbox_load_same->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_label_path->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_label2->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_label3->setStyleSheet(QString("color:rgb(38,38,38);"));
}

void SetupSubtitle::setWidgetFont(QString family, int size)
{
    QFont f(family);
    f.setPointSize(size + 2);
    m_label_title_1->setFont(f);
    m_title2Label->setFont(f);

    f.setPointSize(size);
    m_label_path->setFont(f);
    m_label2->setFont(f);
    m_label3->setFont(f);
    m_familyComboBox->setFont(f);
    m_checkbox_load_same->setFont(f);
    m_sizeComboBox->setFont(f);
    m_browserButton->setFont(f);
    m_subPathEdit->setFont(f);
}

void SetupSubtitle::initLayout()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(30, 10, 24, 8);
    lay->setSpacing(0);

    m_label_title_1 = new QLabel(tr("Sub loading"));
    lay->addWidget(m_label_title_1);

    m_checkbox_load_same = new CheckBox;
    m_checkbox_load_same->setText(tr("Auto loading subtitles with the same name"));
    lay->addWidget(m_checkbox_load_same);

    {
        m_hboxLayout1 = new QHBoxLayout;
        m_hboxLayout1->setContentsMargins(0, 5, 0, 5);
        m_hboxLayout1->setSpacing(10);

        m_label_path = new QLabel(tr("Sub Path"));
        m_label_path->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_hboxLayout1->addWidget(m_label_path);

        m_subPathEdit = new QLineEdit;
        m_hboxLayout1->addWidget(m_subPathEdit);

        m_browserButton = new QPushButton(tr("browser"));
        m_hboxLayout1->addWidget(m_browserButton);

        lay->addLayout(m_hboxLayout1);
    }

    m_title2Label = new QLabel(tr("Font Style"));
    m_title2Label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lay->addWidget(m_title2Label);

    {
        m_hboxLayout2 = new QHBoxLayout;
        m_hboxLayout2->setContentsMargins(0, 5, 0, 5);
        m_hboxLayout2->setSpacing(8);

        m_label2 = new QLabel(tr("Family"));
        m_label2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_hboxLayout2->addWidget(m_label2);

        m_familyComboBox = new QComboBox;
        m_hboxLayout2->addWidget(m_familyComboBox);

        m_hboxLayout2->setStretchFactor(m_familyComboBox, 1);

        lay->addLayout(m_hboxLayout2);
    }

    {
        m_hboxLayout3 = new QHBoxLayout;
        m_hboxLayout3->setContentsMargins(0, 5, 0, 5);
        m_hboxLayout3->setSpacing(8);

        m_label3 = new QLabel(tr("Size"));
        m_label3->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_hboxLayout3->addWidget(m_label3);

        m_sizeComboBox = new QComboBox;
        m_hboxLayout3->addWidget(m_sizeComboBox);

        m_hboxLayout3->setStretchFactor(m_sizeComboBox, 1);

        lay->addLayout(m_hboxLayout3);
    }

    lay->addStretch();
}

void SetupSubtitle::initConnect()
{
    connect(m_checkbox_load_same, &QCheckBox::clicked, [&](bool checked){
        g_config->loadSameNameSub.second = checked;
    });
    connect(m_browserButton, &QPushButton::clicked, [&](){
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
            fd.setDirectory(QDir(m_subPathEdit->text()));
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
            m_subPathEdit->setText(url);
            g_config->subDir.second = url;
        }
    });
    connect(m_familyComboBox, &QComboBox::currentTextChanged, [&](QString family){
        g_config->subFontFamily.second = family;
    });
    connect(m_sizeComboBox, &QComboBox::currentTextChanged, [&](QString size){
        g_config->subFontSize.second = size.toInt();
    });
}

void SetupSubtitle::paintEvent(QPaintEvent *e)
{
    QFontMetrics fontWidth(m_subPathEdit->font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(g_config->subDir.second, Qt::ElideMiddle, m_subPathEdit->width() - m_subPathEdit->font().pointSizeF() * 1.5);
    m_subPathEdit->setText(show_name);
    m_subPathEdit->setToolTip(show_name == g_config->subDir.second ?
                                "" : g_config->subDir.second);
    return QWidget::paintEvent(e);
}
