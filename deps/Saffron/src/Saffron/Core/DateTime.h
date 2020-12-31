#pragma once

#include "Saffron/Base.h"

namespace Se
{

class DateTime
{
	using TimePoint = std::chrono::system_clock::time_point;

public:
	struct Date
	{
		int Seconds;
		int Minutes;
		int Hour;
		int Weekday;
		int Day;
		int Month;
		int Year;
	};

public:
	DateTime();
	explicit DateTime(Date date);
	explicit DateTime(TimePoint timePoint);
	DateTime(int year, int month, int day, int weekday, int hour, int minutes, int seconds);

	bool operator<(const DateTime &rhs) const;
	bool operator>(const DateTime &rhs) const;

	int Seconds() const { return _date.Seconds; }
	int Minutes() const { return _date.Minutes; }
	int Hour() const { return _date.Hour; }
	int Weekday() const { return _date.Weekday; }
	int Day() const { return _date.Day; }
	int Month() const { return _date.Month; }
	int Year() const { return _date.Year; }

	String WeekdayString(bool abbreviation = false) const;
	String MonthString(bool abbreviation = false) const;
	String TimeString() const;
	String ANSIDateString() const;


private:
	void Clamp();

private:
	Date _date{};
};

}
