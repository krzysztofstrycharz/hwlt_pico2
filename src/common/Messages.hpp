#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <cstdint>

namespace hwlt::common {

    /// @brief Flagi opisujące co dokładnie znajduje w poly payload ramki.
enum class PayloadType : uint8_t {
    Empty       = 0x00, // Brak danych (proste komendy)
    Inline8     = 0x01, // Dane mieszczą się w ramce (1 bajt)
    Inline16    = 0x02, // Dane mieszczą się w ramce (2 bajty)
    Inline32    = 0x03, // Dane mieszczą się w ramce (4 bajty)
    Pointer     = 0x04  // Payload jest wskaźnikiem na duży blok pamięci
};

/// @brief 64-bitowa ramka przesyłana przez SPSC Lock-Free Queue
struct alignas(8) CommandFrame {
    uint8_t     processId;    // ID procesu odbierającego ramkę
    uint8_t     commandId;    // ID konkretnej komendy
    PayloadType payloadType;  // Typ pola payload
    uint8_t     reserved;     // Padding dla wyrównania

    // Unia: Zajmuje dokładnie 32 bity pamięci.
    // Interpretacja zależy od payloadType.
    union {
        uint32_t as_u32;
        int32_t  as_i32;
        uint16_t as_u16[2];
        int16_t  as_i16[2];
        uint8_t  as_u8[4];
        int8_t   as_i8[4];
        void*    as_ptr;      // zero-copy dla dużych danych
    } payload;
};

// Standardowe kody błędów (wzorowane na HTTP / statusach systemowych)
enum class StatusCode : uint8_t {
    OK              = 0x00,
    ERROR_GENERAL   = 0x01,
    INVALID_PARAM   = 0x02,
    NOT_SUPPORTED   = 0x03,
    BUSY            = 0x04
};

/// @brief Ramka odpowiedzi wędrująca z powrotem do hosta
struct alignas(8) ResponseFrame {
    uint8_t     processId;     // Kto odpowiada
    uint8_t     commandId;     // Na jaką komendę (tzw. Echo/Correlation ID)
    StatusCode  status;        // Czy się udało?
    PayloadType payloadType;   // Czy zwracamy jakieś dane?
    
    union {
        uint32_t as_u32;
        int32_t  as_i32;
        uint16_t as_u16[2];
        int16_t  as_i16[2];
        uint8_t  as_u8[4];
        int8_t   as_i8[4];
        void*    as_ptr;      // zero-copy dla dużych danych
    } payload;
};

} // namespace hwlt::common

#endif // MESSAGES_HPP