#ifndef IMODULE_HPP
#define IMODULE_HPP

#include <cstdint>
#include "../common/Messages.hpp"

namespace hwlt::modules {

using ModuleId = std::uint8_t;
using EventId  = std::uint16_t;

/// @brief Interfejs bazowy dla wszystkich modułów.
/// Optymalizowany pod względem wysokiej wydajności. 
class IModule {
public:
    virtual ~IModule() = default;

    /// @brief Przetwarzanie komendy sterującej od hosta.
    /// Dane przychodzą przez bezpieczną kolejkę Lock-Free.
    /// Dowolność implementacji zmiany konfiguracji modułu runtime.
    /// Wywoływanie z tego samego rdzenia co process().
    /// @param cmd Zwarty, 64-bitowy pakiet komendy.
    /// @return Ramka odpowiedzi, która zostanie odesłana do Hosta.
    virtual ResponseFrame onCommand(const CommandFrame& cmd) = 0;

    /// @brief Główna pętla logiczna modułu. 
    /// Wywoływana z tego samego rdzenia co onCommand().
    virtual void process() = 0;

} // namespace hwlt::modules

/// @brief Czysto wirtualny interfejs bazowy dla modułów HLT.
/// Zaprojektowany pod kątem zerowego narzutu (zero-overhead) na rdzeniu czasowo krytycznym.
class IModule {
public:
    

    /// @brief Zwraca unikalny identyfikator typu modułu (np. 0x01 dla Analizatora).
    /// Metoda stała, zazwyczaj zwraca wartość zdefiniowaną w czasie kompilacji.
    virtual ModuleId getTypeId() const noexcept = 0;

    /// @brief Inicjalizacja modułu. Tutaj moduł rezerwuje np. piny i DMA.
    /// Wykonywane jednorazowo przy uruchamianiu modułu.
    /// @param resMgr Menedżer zasobów pilnujący konfliktów sprzętowych.
    /// @return true jeśli udało się zarezerwować zasoby, false jeśli wystąpił konflikt.
    virtual bool init(hwlt::system::ResourceManager& resMgr) = 0;

    

    // --- OPCJONALNE METODY DO OBSŁUGI ZDARZEŃ (EventBus) ---
    // Jeśli moduł nasłuchuje zdarzeń sprzętowych od innych modułów.
    // virtual void onEvent(EventId eventId, uint32_t payload) {}
    
    // --- OPCJONALNE METODY DO STRUMIENIOWANIA (DMA -> USB) ---
    // virtual bool canStreamData() const noexcept { return false; }
};

} // namespace hwlt::modules

#endif // IMODULE_HPP
