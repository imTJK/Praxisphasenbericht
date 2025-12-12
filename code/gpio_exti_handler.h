/*
 * gpio_exti_handler.h
 *
 *  Created on: Nov 28, 2024
 *      Author: TK
 */

#ifndef INC_GPIO_EXTI_HANDLER_H_
#define INC_GPIO_EXTI_HANDLER_H_

#include <cstdint>

#ifdef __cplusplus
extern "C++" {
}

#include <unordered_map>

// C++ class definition
class GPIO_EXTI_Handler {
public:
  GPIO_EXTI_Handler(uint16_t GPIO_Pin);
  virtual ~GPIO_EXTI_Handler();

  virtual void handleInterrupt();

  // Static handler for interrupt forwarding
  static void HandleInterruptStatic(uint16_t GPIO_Pin);

private:
  uint16_t pin_;
  static std::unordered_map<uint16_t, GPIO_EXTI_Handler*> handlers; // Internal C++ map
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

// C-compatible function declaration
void GPIO_EXTI_HandleInterrupt(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* INC_GPIO_EXTI_HANDLER_H_ */
