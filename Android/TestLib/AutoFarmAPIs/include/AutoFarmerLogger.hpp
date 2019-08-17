#ifndef AUTOFARMERLOGGER_H
#define AUTOFARMERLOGGER_H

#include <QDebug>

#define LOG_DEBUG qDebug()<<"["<<__FUNCTION__<<"]["<<__LINE__<<"]"
#define LOG_INFO qInfo()<<"["<<__FUNCTION__<<"]["<<__LINE__<<"]"
#define LOG_WARN qWarning()<<"["<<__FUNCTION__<<"]["<<__LINE__<<"]"

#endif // AUTOFARMERLOGGER_H
