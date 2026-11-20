// "Arduino de mentira" para rodar o firmware no PC (tests/test_firmware_sim.cpp).
// O teste controla o relogio e os sensores e observa pinos, PWM e a saida serial.
#pragma once
#include <stdint.h>
#include <cstdlib>
#include <string>
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define A0 14
#define F(x) x

struct Mundo {
  unsigned long ms = 0;
  int dist_cm = 150;           // -1 = sem eco
  int luz = 800;
  bool echo_alto = false;
  int pino[32] = {0};
  int pwm[32] = {0};
  std::string serial;
};
inline Mundo mundo;

inline void pinMode(int, int) {}
inline void digitalWrite(int p, int v) { mundo.pino[p] = v; }
inline int digitalRead(int) { return mundo.echo_alto ? HIGH : LOW; }
inline int analogRead(int) { return mundo.luz; }
inline void analogWrite(int p, int v) { mundo.pwm[p] = v; }
inline unsigned long millis() { return mundo.ms; }
inline void delay(unsigned long) {}
inline void delayMicroseconds(unsigned int) {}
inline long pulseIn(int, int, unsigned long) { return mundo.dist_cm < 0 ? 0 : (long)(mundo.dist_cm * 2 / 0.0343); }

struct SerialMock {
  void begin(long) {}
  void print(const char* s) { mundo.serial += s; }
  void print(char c) { mundo.serial += c; }
  void print(int v) { mundo.serial += std::to_string(v); }
  void print(unsigned int v) { mundo.serial += std::to_string(v); }
  void print(long v) { mundo.serial += std::to_string(v); }
  void print(unsigned long v) { mundo.serial += std::to_string(v); }
  template <class T> void println(T v) { print(v); mundo.serial += '\n'; }
  void println() { mundo.serial += '\n'; }
};
inline SerialMock Serial;
