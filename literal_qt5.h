#ifndef LITERAL_H
#define LITERAL_H

#include "stdafx.h"
#include "staticclass.h"

template <typename DataType, uint VariantType>
class QVariantHolder
{
public:
    constexpr QVariantHolder(DataType value) noexcept :
        data_({ value }), variant_type_(VariantType), is_shared_(0), is_null_(0) {}
    operator QVariant() const noexcept
    {
        static_assert(sizeof(QVariantHolder) == sizeof(QVariant),
            "Inconsistent size, sizeof(QVariantHolder) != sizeof(QVariant)");
        return *reinterpret_cast<const QVariant*>(this);
    }
private:
    union Data
    {
        DataType value_;
        QVariant::Private::Data holder_;
    } data_;
    uint variant_type_ : 30;
    uint is_shared_ : 1;
    uint is_null_ : 1;
};

using StringVariantHolder = QVariantHolder<const uint8_t*, QVariant::String>;
using IntVariantHolder = QVariantHolder<int, QVariant::Int>;

class Literal : public StaticClass
{
private:
    template <typename Char, size_t N>
    struct StaticStringData
    {
    public:
        constexpr StaticStringData() : header_({ { Q_REFCOUNT_INITIALIZE_STATIC, N, 0, HeaderOffset()}}) {};
        constexpr Char& operator[](size_t index) noexcept
        {
            return data_[index];
        }
        constexpr const Char& operator[](size_t index) const noexcept
        {
            return data_[index];
        }
        constexpr Char* GetData() noexcept
        {
            return data_;
        }
        constexpr const uint8_t* GetHeader() const noexcept
        {
            return &header_.dummy_;
        }
    private:
        static constexpr size_t HeaderOffset()
        {
            if constexpr (std::is_same_v<Char, char>)
            {
                return sizeof(QByteArrayData);
            }
            else if constexpr (std::is_same_v<Char, char16_t>)
            {
                return sizeof(QStringData);
            }
            throw std::exception("Invalid char type 'Char'");
        }
        struct ArrayData
        {
            struct RefCount { struct BasicAtomicInt { int ref_; /* AtomicType */ } ref_; } ref_;
            int size_;
            uint bitfield_; // alloc: 31, capacityReserved: 1
            qptrdiff offset_;
        };
        static_assert(sizeof(ArrayData) == sizeof(QArrayData), 
            "Incompatible size, sizeof(ArrayData) != sizeof(QArrayData)");
        union Header
        {
            ArrayData data_;
            uint8_t dummy_;
        } header_;
        Char data_[N + 1] = {};
    };
    template <size_t N>
    struct Utf8ToUtf16
    {
    public:
        template <typename Lambda>
        constexpr Utf8ToUtf16(Lambda lambda)
        {
            auto view = lambda();
            size_t max = view.size();
            size_t position = 0;
            while (position < max)
            {
                uint8_t c1 = view[position++];
                uint8_t c2 = 0, c3 = 0, c4 = 0;
                uint32_t unicode = 0;
                if ((c1 >> 7) == 0)
                {
                    // 0xxxxxxx
                    data_[size_++] = (char16_t)c1;
                }
                else if ((c1 >> 5) == 0b110)
                {
                    if (max - position < 1)
                    {
                        throw std::exception("One extra byte required");
                    }
                    c2 = view[position++];
                    if ((c2 >> 6) != 0b10)
                    {
                        throw std::exception("Invalid continuation byte");
                    }
                    // 110xxxxx 10xxxxxx
                    unicode = ((c1 & 0b000'11111) << 6) |
                        (c2 & 0b00'111111);
                    data_[size_++] = (char16_t)unicode;
                }
                else if ((c1 >> 4) == 0b1110)
                {
                    if (max - position < 2)
                    {
                        throw std::exception("Two extra byte required");
                    }
                    c2 = view[position++];
                    c3 = view[position++];
                    if ((c2 >> 6) != 0b10 || (c3 >> 6) != 0b10)
                    {
                        throw std::exception("Invalid continuation byte");
                    }
                    // 1110xxxx 10xxxxxx 10xxxxxx
                    unicode = ((c1 & 0b0000'1111) << 12) |
                        ((c2 & 0b00'111111) << 6) |
                        (c3 & 0b00'111111);
                    data_[size_++] = (char16_t)unicode;
                }
                else if ((c1 >> 3) == 0b11110)
                {
                    if (max - position < 3)
                    {
                        throw std::exception("Three extra byte required");
                    }
                    c2 = view[position++];
                    c3 = view[position++];
                    c4 = view[position++];
                    if ((c2 >> 6) != 0b10 || (c3 >> 6) != 0b10 || (c4 >> 6) != 0b10)
                    {
                        throw std::exception("Invalid continuation byte");
                    }
                    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    int32_t codePoint = ((c1 & 0b00000'111) << 18) |
                        ((c2 & 0b00'111111) << 12) |
                        ((c3 & 0b00'111111) << 6) |
                        ((c4 & 0b00'111111));
                    if (codePoint > 0xffff)
                    {
                        int32_t value = codePoint - 0x10000;
                        // High surrogate
                        data_[size_++] = (char16_t)((value >> 10) + 0xD800);
                        // Low surrogate
                        data_[size_++] = (char16_t)((value & 0b1111'1111'11) + 0xDC00);
                    }
                    else
                    {
                        data_[size_++] = (char16_t)codePoint;
                    }
                }
                else
                {
                    throw std::exception("Invalid first byte");
                }
            }
        }
        constexpr auto* GetData() const noexcept
        {
            return data_;
        }
        constexpr auto GetSize() const noexcept
        {
            return size_;
        }
    private:
        char16_t data_[N] = {};
        size_t size_ = 0;
    };
    template <typename T>
    static constexpr void Copy(const T* src, size_t size, T* dest) noexcept
    {
        for (auto i = 0; i < size; i++)
        {
            dest[i] = src[i];
        }
    }
    template <typename Lambda>
    static constexpr auto Utf8LiteralData(Lambda lambda)
    {
        constexpr auto view = lambda();
        // The following implementation is not a constant expression
        // std::array<char16_t, view.size()> buffer;
        // constexpr auto size = utf8_to_utf16(buffer.data(), lambda);
        // StaticStringData<char16_t, buffer.size()> utf16;
        // copy(buffer.data(), size, utf16.data());
        constexpr Utf8ToUtf16<view.size()> buffer(lambda);
        StaticStringData<char16_t, buffer.GetSize()> utf16;
        Copy(buffer.GetData(), buffer.GetSize(), utf16.GetData());
        return utf16;
    }
    template <typename Lambda>
    static constexpr auto DirectLiteralData(Lambda lambda)
    {
        constexpr auto view = lambda();
        constexpr size_t size = view.size();
        StaticStringData<typename decltype(view)::value_type, size> data;
        Copy(view.data(), size, data.GetData());
        return data;
    }
public:
    template <typename Lambda>
    static const auto& Utf8Literal(Lambda lambda)
    {
        static constexpr auto literal = Utf8LiteralData(lambda);
        static constexpr auto literal_ptr = &literal;
        return *(QString*)(&literal_ptr);
    }
    template <typename Lambda>
    static const auto& ByteArrayLiteral(Lambda lambda)
    {
        static constexpr auto literal = DirectLiteralData(lambda);
        static constexpr auto literal_ptr = &literal;
        return *(QByteArray*)(&literal_ptr);
    }
    template <typename Lambda>
    static const auto& Utf16Literal(Lambda lambda)
    {
        static constexpr auto literal = DirectLiteralData(lambda);
        static constexpr auto literal_ptr = &literal;
        return *(QString*)(&literal_ptr);
    }
    template <typename Lambda>
    static const auto& Utf16VariantLiteral(Lambda lambda)
    {
        static constexpr auto literal = DirectLiteralData(lambda);
        // reinterpret_cast cannot be used in constexpr context
        static constexpr StringVariantHolder holder = literal.GetHeader();
        return holder;
    }
    template <int value>
    static const auto& IntVariantLiteral()
    {
        static constexpr IntVariantHolder holder = value;
        return holder;
    }
};

// https://mpark.github.io/programming/2017/05/26/constexpr-function-parameters/
#define UTF8_LITERAL(str) Literal::Utf8Literal([] { return std::string_view(str); })
#define STRING_LITERAL(str) Literal::Utf16Literal([] { return std::u16string_view(QT_UNICODE_LITERAL(str)); })
#define BYTE_ARRAY_LITERAL(str) Literal::ByteArrayLiteral([] { return std::string_view(str); })

#define UTF8_POINTER_LITERAL(str) &UTF8_LITERAL(str)
#define STRING_POINTER_LITERAL(str) &STRING_LITERAL(str)
#define BYTE_ARRAY_POINTER_LITERAL(str) &BYTE_ARRAY_LITERAL(str)

#define STRING_VARIANT_LITERAL(str) Literal::Utf16VariantLiteral([] { return std::u16string_view(QT_UNICODE_LITERAL(str)); })
#define INT_VARIANT_LITERAL(value) Literal::IntVariantLiteral<value>()

#endif // LITERAL_H