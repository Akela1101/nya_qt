#ifndef TIMENYA_H
#define TIMENYA_H

#include <QString>
#include <QDateTime>

namespace nya
{
QString GetDurationString(int d);
QString GetTimeString(time_t time);
Qt::GlobalColor GetWeekdayColor(uchar weekday);
char GetTimeZone();
int GetDayFromTime(const QDateTime& dateTime, char timeZone); // deprecated
QDateTime GetTimeFromDay(int day, char timeZone); // deprecated
int GetDayFromTimeE(const QDateTime& dateTime, char timeZone = GetTimeZone());
QDateTime GetTimeFromDayE(int day, char timeZone = GetTimeZone());
}

#endif // TIMENYA_H
