#ifndef CELLDATA_H
#define CELLDATA_H

#include "pch.h"
#include "constants.h"

class CellString
{
public:
    char16_t* GetData() noexcept
    {
        return data_;
    }
    const char16_t* GetData() const noexcept
    {
        return data_;
    }
    void SetLength(quint8 length) noexcept
    {
        length_ = length;
    }
    void CopyTo(QString& s) const noexcept
    {
        s.setUnicode(reinterpret_cast<const QChar*>(GetData()), length_);
    }
private:
    char16_t data_[Constants::kMaxStringLength];
    quint8 length_;
};

struct CellData
{
    CellString odd, even;
};

#endif // CELLDATA_H
