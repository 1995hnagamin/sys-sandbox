#include <bit>
#include <iostream>

namespace {

template<typename T>
constexpr T nprime(T r, T n) {
  T result = 0;
  T t = 0; T e = r; T i = 1;
  while (e > 1) {
    if (!(t % 2)) {
      t += n;
      result += i;
    }
    t /= 2; e /= 2; i *= 2;
  }
  return result;
}

template<uint32_t n>
class Mongomery {
public:
  using T = uint32_t;
  using TP = uint64_t;

private:
  constexpr static T modr(T x) { return x & (r - 1); }
  constexpr static T divr(T x) { return x >> bw; }
  constexpr static T reduce(TP x) {
    T s = modr(x * np);
    T t = divr(x + s * n);
    if (t >= n) {
      t -= n;
    }
    return t;
  }
  constexpr static uint32_t bw = std::bit_width(n);
  constexpr static T r = T(1) << bw;
  constexpr static T q = T(TP(r)*r) % n;
  constexpr static T np = nprime(r, n);

private:
  T num;

public:
  Mongomery(T x): num(reduce(x*q)) {}
  T decrypt() const {
    return reduce(num);
  }

  Mongomery<n>& operator*=(Mongomery<n> const &other) {
    num = reduce(TP(num) * other.num);
    return *this;
  }

  Mongomery<n> & operator+=(Mongomery<n> const &other) {
    num += other.num;
    if (num > n) { num -= n; }
    return *this;
  }
};

template<uint32_t n>
Mongomery<n> operator*(Mongomery<n> const &x, Mongomery<n> const &y) {
  Mongomery<n> m = x;
  m *= y;
  return m;
}

template<uint32_t n>
Mongomery<n> operator+(Mongomery<n> const &x, Mongomery<n> const &y) {
  Mongomery<n> m = x;
  m += y;
  return m;
}

} // namespace

int main() {
  using Mong = Mongomery<169>;
  Mong x(11), y(18);
  Mong z = x * y;
  std::cout << z.decrypt() << "\n";
  return 0;
}
