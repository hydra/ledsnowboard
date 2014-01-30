
int scaleRange(int x, int srcMin, int srcMax, int destMin, int destMax) {

  long int a = ((long int) destMax - (long int) destMin) * ((long int) x - (long int) srcMin);
  long int b = (long int) srcMax - (long int) srcMin;
  return ((a / b) - (destMax - destMin)) + destMax;
}
