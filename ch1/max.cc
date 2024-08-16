// #include <cmath>
#define AMAN 1

namespace John {
  int max_local(int a, int b) { 
    if (a>b && a>AMAN) 
      return a; 
    return b; 
  }
}

int main() {
  return John::max_local(1,2);
}