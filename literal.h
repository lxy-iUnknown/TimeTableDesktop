#ifndef LITERAL_H
#define LITERAL_H

#include "pch.h"

template <typename T>
struct ArrayDataPointer
{
public:
    constexpr ArrayDataPointer(void* d, T* ptr, qsizetype size) noexcept : d(d), ptr(ptr), size(size)
    {
        static_assert(sizeof(ArrayDataPointer<T>) == sizeof(QArrayDataPointer<T>), "Inconsistent size");
    }
private:
    void* d;
    T* ptr;
    qsizetype size;
};

#define BYTE_ARRAY_LITERAL(literal) []() -> const auto& { \
        static constexpr const ArrayDataPointer<char> data = \
        { \
            nullptr, \
            const_cast<char *>(literal), \
            sizeof(literal) / sizeof(char) - 1 \
        }; \
        return reinterpret_cast<const QByteArray&>(data); \
    }()

#define STRING_LITERAL(literal) []() -> const auto& { \
        static constexpr const ArrayDataPointer<char16_t> data = \
        { \
            nullptr, \
            const_cast<char16_t *>(QT_UNICODE_LITERAL(literal)), \
            sizeof(QT_UNICODE_LITERAL(literal)) / sizeof(char16_t) - 1 \
        }; \
        return reinterpret_cast<const QString&>(data); \
    }()\

#define STRING_VIEW_LITERAL(literal) QStringView(QT_UNICODE_LITERAL(literal), \
    sizeof(QT_UNICODE_LITERAL(literal)) / sizeof(char16_t) - 1)

#ifdef QT_DEBUG
#define SET_OBJECT_NAME(control) control->setObjectName(STRING_LITERAL(QT_STRINGIFY(control)))
#else
#define SET_OBJECT_NAME(control)
#endif

#endif // LITERAL_H
