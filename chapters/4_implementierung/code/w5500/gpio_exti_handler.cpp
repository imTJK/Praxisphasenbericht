class GPIO_EXTI_Handler {
public:
    GPIO_EXTI_Handler(uint16_t GPIO_Pin);
    virtual ~GPIO_EXTI_Handler();

    virtual void handleInterrupt();

    // Static handler for interrupt forwarding
    static void HandleInterruptStatic(uint16_t GPIO_Pin);

private:
    uint16_t pin_;
    // Maps GPIO pins to handler instances
    static std::unordered_map<uint16_t, GPIO_EXTI_Handler*> handlers;
};

// Implementation
GPIO_EXTI_Handler::GPIO_EXTI_Handler(uint16_t GPIO_Pin) : pin_(GPIO_Pin) {
    handlers[GPIO_Pin] = this;  // Register handler
}

GPIO_EXTI_Handler::~GPIO_EXTI_Handler() {
    handlers.erase(pin_);  // Unregister handler
}

void GPIO_EXTI_Handler::HandleInterruptStatic(uint16_t GPIO_Pin) {
    auto it = handlers.find(GPIO_Pin);
    if (it != handlers.end()) {
        it->second->handleInterrupt();
    }
}
