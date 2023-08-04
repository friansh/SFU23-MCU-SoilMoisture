class frianshFW_scheduler {
  public:
    void run(unsigned int dataSentInterval, void (*actionToDo)()) {
      if (millis() - lastDataSent > dataSentInterval) {
        (*actionToDo)();
        lastDataSent = millis();
      }
    }
    
  private:
    unsigned long lastDataSent = 0;
};
