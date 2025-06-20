---
title: "CAN message encoding & deconding"
date: 2022-07-12
draft: false
description: "simple demo for can message encoding and deconding."
image: img/cover/math.png
hiddenInList: true
ShowToc: true
TocOpen: true
tags: ["c/cpp", "vehicle"]
categories: ["Algorithm"]
---

## Questions

1. Signals are not byte sized and may span bytes.
2. Signal offsets are arranged as follows: 
```
  7   6   5   4   3   2   1   0
 15  14  13  12  11  10   9   8
 23  22  21  20  19  18  17  16
 31  30  29  28  27  26  25  24
 39  38  37  36  35  34  33  32
 47  46  45  44  43  42  41  40
 55  54  53  52  51  50  49  48
 63  62  61  60  59  58  57  56
```
3. Each signal specifies the value, offset and bit width.


## Encoding
```cpp
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
```