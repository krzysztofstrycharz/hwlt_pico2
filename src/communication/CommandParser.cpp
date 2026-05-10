// Core 0
// Wyciąganie surowych bajtów z bufora kołowego USB.
// Szukanie nagłówka ramki, parsowanie długości, sprawdzanie CRC16.
// Opcjonalne kopiowanie danych do MemoryPool.
// Tworzenie ramki CommandFrame i wciskanie do kolejki SPSC.
// Obsługa odpowiedzi z na rozkazy z kolejki SPSC(?)