#include "kaction.h"

KAction::KAction ( QObject * parent, const char * name, bool autoadd )
    : QAction(parent)
{
    //qDebug("KAction::KAction: name: '%s'", name);
    setObjectName(name);
    if (autoadd) addActionToParent();
}


KAction::KAction( QObject * parent, bool autoadd )
    : QAction(parent)
{
    //qDebug("KAction::KAction: QObject, bool");
    if (autoadd) addActionToParent();
}

KAction::KAction(const QString & text, QKeySequence accel,
                   QObject * parent, const char * name, bool autoadd )
    : QAction(parent)
{
    setObjectName(name);
    setText(text);
    setShortcut(accel);
    if (autoadd) addActionToParent();
}

KAction::KAction(QKeySequence accel, QObject * parent, const char * name,
                   bool autoadd )
    : QAction(parent)
{
    setObjectName(name);
    setShortcut(accel);
    if (autoadd) addActionToParent();
}

KAction::~KAction() {
}

void KAction::addShortcut(QKeySequence key) {
    setShortcuts( shortcuts() << key);
}

void KAction::addActionToParent() {
    if (parent()) {
        if (parent()->inherits("QWidget")) {
            QWidget *w = static_cast<QWidget*> (parent());
            w->addAction(this);
        }
    }
}

void KAction::change(const QIcon & icon, const QString & text) {
    setIcon( icon );
    change(text);
}

void KAction::change(const QString & text ) {
    setText( text );

    QString accel_text = shortcut().toString();

    QString s = text;
    s.replace("&","");
    if (!accel_text.isEmpty()) {
        setToolTip(s + " ("+ accel_text +")");
        setIconText(s);
    }
}

