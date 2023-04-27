#ifndef DATE_UTILITY_H
#define DATE_UTILITY_H

#include "pch.h"
#include "utility.h"

class DateUtility {
private:
    template<typename Int>
    static constexpr Int DivideRoundDown(Int a, unsigned int b)
    {
        if (a < 0)
        {
            return (a + 1) / static_cast<Int>(b) - 1;
        }
        return a / static_cast<Int>(b);
    }
    template<typename Int>
    static constexpr Int ModuloRoundDown(Int a, unsigned int b)
    {
        return a - DivideRoundDown(a, b) * b;
    }
    static constexpr bool IsLeapYear(int year)
    {
        if (year < 1)
        {
            ++year;
        }
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    }
    static constexpr bool ValidateYMD(int year, int month, int day)
    {
        constexpr int DaysInMonth[2][12] =
        {
            { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
            { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        };
        
        if (year == 0)
        {
            return false;
        }
        if (month < 0 || month > 12)
        {
            return false;
        }
        return day > 0 && day <= DaysInMonth[IsLeapYear(year)][month - 1];
    }
    // Algorithm modified from https://blog.csdn.net/xqyunyun/article/details/115191308
    static int WeekCountInternal(const QDate& start, const QDate& end)
    {
        constexpr int kDaysInWeek = 7;
        constexpr int kMonday = static_cast<int>(Qt::DayOfWeek::Monday);
        constexpr int kSunday = static_cast<int>(Qt::DayOfWeek::Sunday);

        int start_julian_day = static_cast<int>(start.toJulianDay());
        int end_julian_day = static_cast<int>(end.toJulianDay());
        // Equivalent code:
        // // Days between start date and previous first day of week
        // int d1 = start_julian_day - (start_julian_day % kDaysInWeek + 1) + kMonday;
        // // Days between end date and following first day of week
        // int d2 = end_julian_day + kSunday - (end_julian_day % kDaysInWeek + 1);
        // return 1 + (d2 - d1) / kDaysInWeek;
        return 1 + ((end_julian_day - end_julian_day % kDaysInWeek) -
                    (start_julian_day - start_julian_day % kDaysInWeek) +
                    (kSunday - kMonday)) / kDaysInWeek;
    }
public:
    static constexpr qint64 JulianDayFromYMD(int year, int month, int day)
    {
        if (!ValidateYMD(year, month, day))
        {
            return Utility::ConstExprError<qint64>("Invalid year, month or day");
        }
        if (year < 0)
        {
            ++year;
        }
        int a = month < 3 ? 1 : 0;
        qint64 y = static_cast<qint64>(year) + 4800 - a;
        int m = month + 12 * a - 3;
        return day + DivideRoundDown(153 * m + 2, 5) - 32045 + 365 * y + 
            DivideRoundDown(y, 4) - DivideRoundDown(y, 100) + DivideRoundDown(y, 400);
    }
    static int WeekCount(const QDate& start, const QDate& end)
    {
        if (start <= end)
        {
            return WeekCountInternal(start, end);
        }
        return -WeekCountInternal(end, start);
    }
};

#endif // DATE_UTILITY_H
