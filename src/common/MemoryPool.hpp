#ifndef MEMORYPOOL_HPP
#define MEMORYPOOL_HPP

#include <cstdint>
#include <cstddef>
#include <pico/sync.h>

namespace hwlt::common {

/// @brief Pre-alokowana pula pamięci do bezkopiowego (zero-copy) przesyłania danych między rdzeniami.
/// Składa się z 32 bloków po 256 bajtów. Wspiera alokację ciągłą (wieloblokową).
class MemoryPool {
public:
    static constexpr std::size_t BLOCK_SIZE = 256;
    static constexpr std::size_t NUM_BLOCKS = 32;

    /// @brief Inicjalizuje struktury synchronizacji (wymagane wywołanie w main przed startem rdzeni)
    static void init();

    /// @brief Alokuje określoną liczbę bajtów z puli.
    /// @param bytes Liczba bajtów do zaalokowania.
    /// @return Wskaźnik na pamięć lub nullptr, jeśli brakuje ciągłego miejsca.
    static void* allocate(std::size_t bytes);

    /// @brief Zwalnia wcześniej zaalokowaną pamięć.
    /// @param ptr Wskaźnik uzyskany z funkcji allocate().
    static void free(void* ptr);

private:
    // Pamięć musi być wyrównana do 8 bajtów, aby uniknąć błędów sprzętowych na ARM (Hard Fault)
    // przy rzutowaniu na większe struktury lub obsłudze DMA.
    alignas(8) static uint8_t memory_[NUM_BLOCKS * BLOCK_SIZE];

    // Wykorzystujemy 32 bitową zmienną jako mapę zajętości 32 bloków pamięci.
    // Mapa zajętości: 1 = blok zajęty, 0 = blok wolny.
    static uint32_t bitmap_;

    // Tablica przechowująca informację, ile bloków zajęła dana alokacja (potrzebne przy zwalnianiu).
    static uint8_t allocationSize_[NUM_BLOCKS];

    // Sekcja krytyczna Pico SDK chroniąca operacje na bitach z obu rdzeni.
    static critical_section_t critSec_;
};

} // namespace hwlt::common

#endif // MEMORYPOOL_HPP