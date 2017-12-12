#include <functional>
#include <iostream>

class C {
public:
  mutable int n;
  int & k;
  void f(std::function<void(int&)> g) const {
    g(n);
  }

  C(int k) : n(k) {}
};

void g1 (int &q) {q=q+1;}
void g2 (const int&q) {q+1;}

int main () {
  int a = 23849;
  const int * const y = &a;
  C p = C(3);
  p.f (g1);
  std::cout << p.n << std::endl;
  return 0;
}

