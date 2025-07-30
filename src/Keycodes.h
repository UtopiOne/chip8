#pragma once

namespace Chip8 {

enum class Keycode {
  One = 30,
  Two = 31,
  Three = 32,
  Four = 33,

  Q = 20,
  W = 26,
  E = 8,
  R = 21,

  A = 4,
  S = 22,
  D = 7,
  F = 9,

  Z = 29,
  X = 27,
  C = 6,
  V = 25,
};

/*
 * 1 2 3 C    1 2 3 4
 * 4 5 6 D -> Q W E R
 * 7 8 9 E    A S D F
 * A 0 B F    Z X C V
 * */
inline Keycode HexToKey(int hex_val) {
  switch (hex_val) {
    case 0x0: {
      return Keycode::X;
      break;
    }
    case 0x1: {
      return Keycode::One;
      break;
    }
    case 0x2: {
      return Keycode::Two;
      break;
    }
    case 0x3: {
      return Keycode::Three;
      break;
    }
    case 0x4: {
      return Keycode::Q;
      break;
    }
    case 0x5: {
      return Keycode::W;
      break;
    }
    case 0x6: {
      return Keycode::E;
      break;
    }
    case 0x7: {
      return Keycode::A;
      break;
    }
    case 0x8: {
      return Keycode::S;
      break;
    }
    case 0x9: {
      return Keycode::D;
      break;
    }
    case 0xA: {
      return Keycode::Z;
      break;
    }
    case 0xB: {
      return Keycode::C;
      break;
    }
    case 0xC: {
      return Keycode::Four;
      break;
    }
    case 0xD: {
      return Keycode::R;
      break;
    }
    case 0xE: {
      return Keycode::F;
      break;
    }
    case 0xF: {
      return Keycode::V;
      break;
    }

    default: {
      return Keycode::One;
      break;
    }
  }
}

}  // namespace Chip8
