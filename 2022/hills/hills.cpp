#include <array>
#include <iostream>
#include <string>
#include <vector>

size_t constexpr Width = 5;
size_t constexpr Base = 27;
std::string const Digits = "abcdefghijklmnopqrstuvwxyz*";

int from_chars(std::string_view sv) {
  int val = 0;
  for (char c : sv) {
    val *= Base;
    val += (c == '*') ? (Base-1) : (c-'a');
  }
  return val;
}

std::string to_chars(int val) {
  std::array<char, Width+1> arr;
  for (int i = Width-1; i >= 0; --i) {
    arr[i] = Digits[val % Base];
    val /= Base;
  }
  arr[Width] = '\0';
  return arr.data();
}

size_t constexpr SIZE = 14348907; // (26+1)^5
int count[SIZE];

int main() {
  std::string line;
  while (std::cin >> line) {
    for (int i = 0; i < Width; ++i) {
      std::string s = line;
      s[i] = '*';
      count[from_chars(s)] += 1;
    }
  }

  int const threshold = 7;
  for (int val = 0; val < SIZE; ++val) {
    if (count[val] >= threshold) {
      std::cout << to_chars(val) << "\t" << count[val] << "\n";
    }
  }

  return 0;
}
