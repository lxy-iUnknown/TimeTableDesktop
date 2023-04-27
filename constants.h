#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "pch.h"
#include "literal.h"
#include "date_utility.h"

namespace Constants
{
    // Unix epoch
    inline static constexpr int kMinYear = 1970;
    inline static constexpr int kMinMonth = 1;
    inline static constexpr int kMinDay = 1;

    inline static constexpr int kMaxYear = 9999;
    inline static constexpr int kMaxMonth = 12;
    inline static constexpr int kMaxDay = 31;

    inline static constexpr QDate kMinDate = QDate::fromJulianDay(
                DateUtility::JulianDayFromYMD(kMinYear, kMinMonth, kMinDay));
    inline static constexpr QDate kMaxDate = QDate::fromJulianDay(
                DateUtility::JulianDayFromYMD(kMaxYear, kMaxMonth, kMaxDay));

    inline static constexpr quint32 kMinDateJulianDay = static_cast<quint32>(kMinDate.toJulianDay());
    inline static constexpr quint32 kMaxDateJulianDay = static_cast<quint32>(kMaxDate.toJulianDay());

    inline static constexpr qint32 kMaxDateEpochDay = static_cast<qint32>(kMaxDateJulianDay - kMinDateJulianDay);

    inline static constexpr qint32 kInvalidDateEpochDay = -1;

    inline static constexpr int kRowCount = 12;
    inline static constexpr int kColumnCount = 5;
    inline static constexpr int kCellCount = kRowCount * kColumnCount;

    inline static constexpr int kMaxStringLength = (1 << (CHAR_BIT * sizeof(quint8))) - 1;

    inline static const QString& kEmptyString = STRING_LITERAL("");

    inline static const QString& kPromptToSave = STRING_LITERAL("是否保存更改？");

    inline static const QString& kWarning = STRING_LITERAL("提示");
    inline static const QString& kError = STRING_LITERAL("错误");

    inline static const QString& kUntitled = STRING_LITERAL("未命名.bin");
}

#endif // CONSTANTS_H
