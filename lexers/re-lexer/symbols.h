
#include <stdint.h>
#include <string>

namespace lexer {

//! Преобразование из UTF-16 в CP 1251.
inline char GetCp1251FromUtf16(uint16_t code) {
  if (code >= 0x0410 and code <= 0x042F) { // Прописные для кирилицы.
    return code - 0x0410 + 0xc0;
  } else if (code >= 0x0430 and code <= 0x044F) { // Строчные для кирилицы.
    return code - 0x0430 + 0xe0;
  } else if (code == 0x0401) { // Прописная Ё.
    return '\xa8';
  } else if (code == 0x0451) { // Строчная ё.
    return '\xb8';
  } else if (code < 0x80) { // ANSI символы.
    return code;
  }
  return '\0';
}

//! Преобразование из CP-1251 в UTF-16.
inline uint16_t GetUtf16FromCp1251(char ch_code) {
  uint16_t code = (uint16_t)(uint8_t)ch_code;
  if (code >= 0xc0 and code <= 0xdf) { // Прописные для кирилицы.
    return code + 0x0410 - 0xc0;
  } else if (code >= 0xe0 and code <= 0xff) { // Строчные для кирилицы.
    return code - 0xe0 + 0x0430;
  } else if (code == 0xa8) { // Прописная Ё.
    return 0x0401;
  } else if (code == 0xb8) { // Строчная ё.
    return 0x0451;
  } else if (code < 0x80) { // ANSI символы.
    return code;
  }
  return 0;
}

//! Преобразование UTF-16 кода в UTF-8 последовательность.
inline void GetUtf8Sequence(uint16_t code, std::string& seq) {
    const uint16_t kByteMask       = 0xbfu;
    const uint16_t kByteMark       = 0x80u;
    const uint16_t kTwoByteMark    = 0xc0u;
    const uint16_t kThreeByteMark  = 0xe0u;

    if (code < 0x80u) {
        seq += code;
    } else if (code < 0x800u) {
        char sbyte = (code | kByteMark) & kByteMask;
        code >>= 6;
        char fbyte = code | kTwoByteMark;

        seq += fbyte;
        seq += sbyte;
    } else {
        char tbyte = (code | kByteMark) & kByteMask;
        code >>= 6;
        char sbyte = (code | kByteMark) & kByteMask;
        code >>= 6;
        char fbyte = (code | kThreeByteMark);

        seq += fbyte;
        seq += sbyte;
        seq += tbyte;
    }
}

}  // namespace lexer

