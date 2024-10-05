#include "sidebardelegate.h"
#include "global/variable.h"
#include <QDebug>
SideBarDelegate::SideBarDelegate() {}

QSize SideBarDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    QSize tempSize;
    if (Variable::g_mainlineVersion) {
        tempSize = QSize(94 + 6, 56 + 6);
    } else {
        tempSize = QSize(96, 150);
    }
    return tempSize;
}
