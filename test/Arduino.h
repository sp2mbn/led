#ifndef Arduino_h
#define Arduino_h

#include <cstdint>
#include <functional>

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define LED_BUILTIN 13

#define F(x) x

class HardwareSerial {
public:
    void println(const char* s) {}
    void print(const char* s) {}
};

inline HardwareSerial Serial;

inline void pinMode(uint8_t pin, uint8_t mode) { (void)pin; (void)mode; }
inline void digitalWrite(uint8_t pin, uint8_t val) { (void)pin; (void)val; }
inline uint8_t digitalRead(uint8_t pin) { (void)pin; return 0; }
inline void analogWrite(uint8_t pin, int val) { (void)pin; (void)val; }
inline int analogRead(uint8_t pin) { (void)pin; return 0; }

inline void delay(unsigned long ms) { (void)ms; }

unsigned long millis();
void delay(unsigned long ms);

template<typename T, typename U, typename V, typename W, typename X>
auto map(T x, U in_min, V in_max, W out_min, X out_max) -> decltype((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define log_e(...) 
typedef bool boolean;

#endif