#include "appsw.hpp"

uint32_t calculateCRC32Byte(uint32_t crc, uint8_t data) {
    crc ^= data;
    for (int i = 0; i < 8; ++i) {
        if (crc & 1) {
            crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
        } else {
            crc >>= 1;
        }
    }
    return crc;
}

// Funzione per calcolare il CRC32 di un buffer di dati
uint32_t calculateCRC32(const char* data, size_t length) {
    uint32_t crc = CRC32_INITIAL_VALUE;
    for (size_t i = 0; i < length; ++i) {
        crc = calculateCRC32Byte(crc, static_cast<uint8_t>(data[i]));
    }
    return crc ^ CRC32_INITIAL_VALUE;
}

uint32_t getFileCRC32(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Impossibile aprire il file: " << filename << std::endl;
        return 0;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        std::cerr << "Errore durante la lettura del file: " << filename << std::endl;
        return 0;
    }

    file.close();

    return calculateCRC32(buffer.data(), buffer.size());
}