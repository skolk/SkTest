class Valve {
  public:
    Valve(uint8_t pinOpen, uint8_t pinClose);
    void open();
    void close();
  private:
    uint8_t PIN_OPEN;
    uint8_t PIN_CLOSE;
}