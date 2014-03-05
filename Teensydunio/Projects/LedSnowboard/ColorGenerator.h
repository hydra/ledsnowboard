#ifndef COLORGENERATOR_H
#define COLORGENERATOR_H

class ColorGenerator {
public:
  virtual RGB_t &generate(void) = 0;
};

#endif
