#include "SaffronPCH.h"

#include "Saffron/Core/DateTime.h"
#include "Saffron/Libraries/GenUtils.h"

namespace Se
{
////////////////////////////////////////////////////////////////
/// Helper functions
////////////////////////////////////////////////////////////////

static String ValueToMonthString(Int32 month)
{
	switch ( month )
	{
	case 0:  return "January";
	case 1:  return "February";
	case 2:  return "March";
	case 3:  return "April";
	case 4:  return "May";
	case 5:  return "June";
	case 6:  return "July";
	case 7:  return "August";
	case 8:  return "September";
	case 9:  return "October";
	case 10: return "November";
	case 11: return "December";
	default: return "Invalid";
	}
}

static String ValueToMonthShortString(Int32 month)
{
	switch ( month )
	{
	case 0:  return "Jan";
	case 1:  return "Feb";
	case 2:  return "Mar";
	case 3:  return "Apr";
	case 4:  return "May";
	case 5:  return "Jun";
	case 6:  return "Jul";
	case 7:  return "Aug";
	case 8:  return "Sep";
	case 9: return "Oct";
	case 10: return "Nov";
	case 11: return "Dec";
	default: return "Inv";
	}
}

static String ValueToWeekdayString(Int32 weekday)
{
	switch ( weekday )
	{
	case 0:  return "Monday";
	case 1:  return "Tuesday";
	case 2:  return "Wednesday";
	case 3:  return "Thursday";
	case 4:  return "Friday";
	case 5:  return "Saturday";
	case 6:  return "Sunday";
	default: return "Invalid";
	}
}

static String ValueToWeekdayShortString(Int32 weekday)
{
	switch ( weekday )
	{
	case 0:  return "Mon";
	case 1:  return "Tue";
	case 2:  return "Wed";
	case 3:  return "Thu";
	case 4:  return "Fri";
	case 5:  return "Sat";
	case 6:  return "Sun";
	default: return "Inv";
	}
}

////////////////////////////////////////////////////////////////
/// DateTime
////////////////////////////////////////////////////////////////

DateTime::DateTime()
	: DateTime(std::chrono::system_clock::now())
{
}

DateTime::DateTime(Date date)
	: _date(Move(date))
{
	Clamp();
}

DateTime::DateTime(TimePoint timePoint)
{
	time_t time = std::chrono::system_clock::to_time_t(timePoint);
	const std::tm *tm = std::localtime(&time);
	_date = { tm->tm_sec, tm->tm_min, tm->tm_hour, tm->tm_wday - 1,tm->tm_mday - 1, tm->tm_mon, tm->tm_year + 1900 };
	Clamp();
}

DateTime::DateTime(int year, int month, int day, int weekday, int hour, int minutes, int seconds)
	: _date({ seconds, minutes, hour, weekday, day, month, year })
{
	Clamp();
}

bool DateTime::operator<(const DateTime &rhs) const
{
	const auto &lDate = _date;
	const auto &rDate = rhs._date;

	return
		std::make_tuple(lDate.Year, lDate.Month, lDate.Day, lDate.Weekday, lDate.Hour, lDate.Minutes, lDate.Seconds) <
		std::make_tuple(rDate.Year, rDate.Month, rDate.Day, rDate.Weekday, rDate.Hour, rDate.Minutes, rDate.Seconds);
}

bool DateTime::operator>(const DateTime &rhs) const
{
	const auto &lDate = _date;
	const auto &rDate = rhs._date;

	return
		std::make_tuple(lDate.Year, lDate.Month, lDate.Day, lDate.Weekday, lDate.Hour, lDate.Minutes, lDate.Seconds) >
		std::make_tuple(rDate.Year, rDate.Month, rDate.Day, rDate.Weekday, rDate.Hour, rDate.Minutes, rDate.Seconds);
}

String DateTime::WeekdayString(bool abbreviation) const
{
	return abbreviation ? ValueToWeekdayShortString(Weekday()) : ValueToWeekdayString(Weekday());
}

String DateTime::MonthString(bool abbreviation) const
{
	return abbreviation ? ValueToMonthShortString(Month()) : ValueToMonthString(Month());
}

String DateTime::TimeString() const
{
	OutputStringStream oss;
	oss << std::setw(2) << std::setfill('0') << Hour() << ":" << std::setw(2) << std::setfill('0') << Minutes();
	return oss.str();
}

String DateTime::ANSIDateString() const
{
	OutputStringStream oss;
	oss << Year() << "-" <<
		std::setw(2) << std::setfill('0') << Month() << "-" <<
		std::setw(2) << std::setfill('0') << Day();
	return oss.str();
}

void DateTime::Clamp()
{
	GenUtils::Clamp(_date.Seconds, 0, 59);
	GenUtils::Clamp(_date.Seconds, 0, 59);
	GenUtils::Clamp(_date.Hour, 0, 23);
	GenUtils::Clamp(_date.Weekday, 0, 6);
	GenUtils::Clamp(_date.Day, 0, 30);
	GenUtils::Clamp(_date.Month, 0, 11);
}
}
