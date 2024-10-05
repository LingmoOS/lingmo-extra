#ifndef KACTION_H
#define KACTION_H

#include <QAction>

class KAction : public QAction
{
    Q_OBJECT
public:
    //! Creates a new KAction with name \a name. If \a autoadd is true
    //! the action will be added to the parent
    KAction ( QObject * parent, const char * name, bool autoadd = true );

    //! Creates a new KAction. If \a autoadd is true
    //! the action will be added to the parent
    KAction ( QObject * parent, bool autoadd = true );

    KAction ( const QString & text, QKeySequence accel,
               QObject * parent, const char * name = "",
               bool autoadd = true );

    KAction ( QKeySequence accel, QObject * parent,
               const char * name = "", bool autoadd = true );

    ~KAction();

    void addShortcut(QKeySequence key);

    //! Change the icon and text of the action.
    void change(const QIcon & icon, const QString & text );

    //! Change the text of the action.
    void change(const QString & text);

protected:
    //! Checks if the parent is a QWidget and adds the action to it.
    void addActionToParent();
};

#endif // KACTION_H
