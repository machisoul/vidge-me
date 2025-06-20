#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>

void MsgEncoding64(uint64_t &msg, uint64_t const &signal, uint8_t const offset,
                   uint8_t const bit_width) {
  constexpr uint8_t bits_per_byte{8U};
  constexpr uint8_t bitmask_size{64U};

  uint64_t value_mask = ((1ULL << bit_width) - 1) & signal;

  for (int i = 0; i < bit_width; i++) {
    uint64_t current_bit_value = (value_mask >> i) & 1ULL;
    uint8_t bit_postion_in_byte = (offset + i) % bits_per_byte;
    uint8_t byte_index = (offset + i) / bits_per_byte;
    msg |= current_bit_value
           << (bitmask_size - (byte_index + 1) * bits_per_byte +
               bit_postion_in_byte);
  }
}

void printBinary64(uint64_t signal) {
  std::bitset<64> bits(signal);
  std::string binary = bits.to_string();
  for (size_t i = 0; i < binary.length(); i += 8) {
    std::cout << binary.substr(i, 8) << std::endl;
  }
}

int main() {
  uint64_t msg = 0U;

  // case 1: first data;
  uint64_t signal = 0b1101;
  uint8_t offset = 0U;
  uint8_t bit_width = 4U;
  MsgEncoding64(msg, signal, offset, bit_width);
  std::cout << "Case 1 - first data:" << std::endl;
  printBinary64(msg);
  /*
  case 1 result:
  00001101 <---
  00000000
  00000000
  00000000
  00000000
  00000000
  00000000
  00000000
  */

  // case 2: last data;
  msg = 0U;
  signal = 0b1001;
  offset = 60U;
  bit_width = 4U;
  MsgEncoding64(msg, signal, offset, bit_width);
  std::cout << "Case 2 - last data:" << std::endl;
  printBinary64(msg);
  /*
  case 2 result:
  00000000
  00000000
  00000000
  00000000
  00000000
  00000000
  00000000
  10010000 <---
  */

  // case 3: Cross-byte;
  msg = 0U;
  signal = 0b10111;
  offset = 6U;
  bit_width = 5U;
  MsgEncoding64(msg, signal, offset, bit_width);
  std::cout << "Case 3 - Cross-byte:" << std::endl;
  printBinary64(msg);
  /*
  case 3 result:
  11000000 <---
  00000101 <---
  00000000
  00000000
  00000000
  00000000
  00000000
  00000000
  */

  return 0;
}