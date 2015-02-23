/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#include "Nya.hpp"

#include <map>

#include "TimeNya.hpp"


namespace Nya
{
/**
 * Упрощённая строка длительности.
 */
QString GetDurationString(int d)
{
	int day = d / 86400; d -= day * 86400;
	int hour = d / 3600; d -= hour * 3600;
	int min = d / 60;
	return QString("%1 %2:%3").arg(day, 2).arg(hour, 2, 10, QChar('0')).arg(min, 2, 10, QChar('0'));
}

/**
 * Строка времени.
 */
QString GetTimeString(time_t time)
{
	return QDateTime::fromTime_t(time).toString(NYA_TIME_FORMAT);
}

/**
 * Цвет дня недели.
 */
Qt::GlobalColor GetWeekdayColor(uchar weekday)
{
	static std::map<uchar, Qt::GlobalColor> colors =
	{
//        { 1, Qt::darkGray },
//        { 2, Qt::red },
//        { 3, Qt::cyan },
//        { 4, Qt::green },
//        { 5, Qt::yellow },
//        { 6, Qt::darkYellow },
//        { 7, Qt::white }
		{ 1, Qt::gray },
		{ 2, Qt::gray },
		{ 3, Qt::gray },
		{ 4, Qt::gray },
		{ 5, Qt::gray },
		{ 6, Qt::darkRed },
		{ 7, Qt::darkRed }
	};
	return colors[weekday];
}

/**
 * Получить часовой пояс.
 */
char GetTimeZone()
{
	QDateTime dt1 = QDateTime::currentDateTime();
	QDateTime dt2 = dt1; dt2.setTimeSpec(Qt::UTC);
	return char(dt1.secsTo(dt2) / 3600);
}

/**
 * Дни от 1970 года из времени.
 * Если не переводить время в UTC будет лажа,
 * связанная с историческим изменением времени в разных часовых поясах.
 *
 * +1 -1 день можно стереть, только тогда надо всю базу менять в LWM.
 *
 * day_number = 1 + t + Δt - t₀
 */
int GetDayFromTime(const QDateTime& dateTime, char timeZone)
{
	return QDateTime::fromTime_t(0).toUTC().daysTo(dateTime.toUTC().addSecs(3600 * timeZone)) + 1;
}

/**
 * Время полуночи из дней, считая от 1970 года.
 *
 * t = t₀ + day_number - 1 - Δt
 */
QDateTime GetTimeFromDay(int day, char timeZone)
{
	return QDateTime::fromTime_t(0).toUTC().addDays(day - 1).addSecs(-3600 * timeZone).toLocalTime();
}

/**
 * Дни от 1970 года из времени.
 * Если не переводить время в UTC будет лажа,
 * связанная с историческим изменением времени в разных часовых поясах.
 *
 * day_number = t + Δt - t₀
 */
int GetDayFromTimeE(const QDateTime& dateTime, char timeZone)
{
	return QDateTime::fromTime_t(0).toUTC().daysTo(dateTime.toUTC().addSecs(3600 * timeZone));
}

/**
 * Время полуночи из дней, считая от 1970 года.
 *
 * t = t₀ + day_number - Δt
 */
QDateTime GetTimeFromDayE(int day, char timeZone)
{
	return QDateTime::fromTime_t(0).toUTC().addDays(day).addSecs(-3600 * timeZone).toLocalTime();
}
}
