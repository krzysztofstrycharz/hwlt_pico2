#ifndef SPSCQUEUE_HPP
#define SPSCQUEUE_HPP

#include <atomic>
#include <cstddef>
#include <array>

namespace hwlt::common {

/// @brief Szybka, bezblokująca kolejka jednokierunkowa (Single-Producer Single-Consumer).
/// Bezpieczna do przesyłania danych między dwoma dedykowanymi rdzeniami.
/// @tparam T Typ przesyłanej struktury (np. CommandFrame).
/// @tparam Capacity Maksymalna pojemność kolejki (Musi być potęgą liczby 2!).
template <typename T, std::size_t Capacity>
class SpscQueue {
    // Zabezpieczenie kompilatora: Pojemność musi być potęgą dwójki (np. 16, 32, 64)
    static_assert((Capacity > 0) && ((Capacity & (Capacity - 1)) == 0),
                  "Capacity of SpscQueue must be a power of 2 for bitwise optimization!");

public:
    SpscQueue() = default;

    /// @brief Próbuje dodać element do kolejki (Wywoływane przez Producenta).
    /// @param item Obiekt do skopiowania.
    /// @return true, jeśli dodano. false, jeśli kolejka jest pełna.
    bool push(const T& item) noexcept {
        const std::size_t current_write = write_idx_.load(std::memory_order_relaxed);
        const std::size_t next_write = (current_write + 1) & (Capacity - 1); // Błyskawiczne modulo

        // Sprawdzamy, czy wskaźnik zapisu nie dogonił wskaźnika odczytu (Kolejka pełna)
        if (next_write == read_idx_.load(std::memory_order_acquire)) {
            return false; 
        }

        buffer_[current_write] = item;

        // Publikujemy nowy wskaźnik zapisu. 'release' gwarantuje, 
        // że dane w buffer_ są już widoczne w RAM, zanim zaktualizujemy write_idx_.
        write_idx_.store(next_write, std::memory_order_release);
        return true;
    }

    /// @brief Próbuje pobrać element z kolejki (Wywoływane przez Konsumenta).
    /// @param[out] item Zmienna, do której zostanie skopiowany element.
    /// @return true, jeśli pobrano. false, jeśli kolejka jest pusta.
    bool pop(T& item) noexcept {
        const std::size_t current_read = read_idx_.load(std::memory_order_relaxed);

        // Sprawdzamy, czy jest coś do odczytania
        if (current_read == write_idx_.load(std::memory_order_acquire)) {
            return false; // Kolejka pusta
        }

        item = buffer_[current_read];

        // Przesuwamy wskaźnik odczytu. 'release' mówi producentowi, że zwolniliśmy miejsce.
        read_idx_.store((current_read + 1) & (Capacity - 1), std::memory_order_release);
        return true;
    }

private:
    std::array<T, Capacity> buffer_{};

    // alignas(64) zapobiega zjawisku "False Sharing" w pamięci podręcznej (Cache L1).
    // Upewniamy się, że wskaźniki odczytu i zapisu leżą w różnych liniach cache.
    alignas(64) std::atomic<std::size_t> write_idx_{0};
    alignas(64) std::atomic<std::size_t> read_idx_{0};
};

} // namespace hwlt::common

#endif // SPSCQUEUE_HPP