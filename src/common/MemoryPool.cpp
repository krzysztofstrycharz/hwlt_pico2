#include "MemoryPool.hpp"

namespace hwlt::common {

// Inicjalizacja pól statycznych
alignas(8) uint8_t MemoryPool::memory_[NUM_BLOCKS * BLOCK_SIZE] = {0};
uint32_t MemoryPool::bitmap_ = 0;
uint8_t MemoryPool::allocationSize_[NUM_BLOCKS] = {0};
critical_section_t MemoryPool::critSec_;

void MemoryPool::init() {
    critical_section_init(&critSec_);
}

void* MemoryPool::allocate(std::size_t bytes) {
    if (bytes == 0) return nullptr;

    // Obliczamy ile bloków potrzebujemy (sufit z dzielenia)
    uint8_t blocksNeeded = (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (blocksNeeded > NUM_BLOCKS) return nullptr;

    // Tworzymy maskę bitową, np. dla 3 bloków to będzie binarne 0111 (dec: 7)
    uint32_t mask = (1ULL << blocksNeeded) - 1;

    void* allocatedPtr = nullptr;

    // --- WEJŚCIE DO SEKCJI KRYTYCZNEJ (Blokuje przerwania i inne rdzenie) ---
    critical_section_enter_blocking(&critSec_);

    // Szukamy wolnego miejsca przesuwając naszą maskę po 32 bitach (Sliding Window)
    for (uint8_t i = 0; i <= (NUM_BLOCKS - blocksNeeded); ++i) {
        // Jeśli pod naszą maską wszystkie bity to 0, mamy wolne, ciągłe miejsce!
        if ((bitmap_ & (mask << i)) == 0) {
            
            // Oznaczamy bity jako zajęte (1)
            bitmap_ |= (mask << i);
            
            // Zapisujemy, że pod tym indeksem zaczyna się alokacja o długości 'blocksNeeded'
            allocationSize_[i] = blocksNeeded;
            
            // Obliczamy fizyczny wskaźnik pamięci
            allocatedPtr = &memory_[i * BLOCK_SIZE];
            break;
        }
    }

    critical_section_exit(&critSec_);
    // --- WYJŚCIE Z SEKCJI KRYTYCZNEJ ---

    return allocatedPtr;
}

void MemoryPool::free(void* ptr) {
    if (!ptr) return;

    // Obliczamy index bloku na podstawie wskaźnika
    std::ptrdiff_t offset = static_cast<uint8_t*>(ptr) - memory_;
    
    // Zabezpieczenie przed obcym/błędnym wskaźnikiem
    if (offset < 0 || offset >= static_cast<std::ptrdiff_t>(sizeof(memory_)) || (offset % BLOCK_SIZE) != 0) {
        return; 
    }

    uint8_t index = offset / BLOCK_SIZE;

    critical_section_enter_blocking(&critSec_);

    // Odczytujemy, z ilu bloków składała się ta alokacja
    uint8_t blocksToFree = allocationSize_[index];
    
    if (blocksToFree > 0) {
        // Tworzymy odpowiednią maskę i zerujemy te bity, uwalniając pamięć
        uint32_t mask = (1ULL << blocksToFree) - 1;
        bitmap_ &= ~(mask << index);
        
        // Czyścimy informację o rozmiarze
        allocationSize_[index] = 0;
    }

    critical_section_exit(&critSec_);
}

} // namespace hwlt::common