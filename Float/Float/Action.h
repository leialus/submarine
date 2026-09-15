#ifndef MyAction
#define MyAction

struct  ActionPackage {
  uint8_t typeMessage;
  uint8_t button;
  bool action;

   // Construtor
    ActionPackage(uint8_t message, uint8_t btn, bool act)
        : typeMessage(message), button(btn), action(act) {}
};

#endif
