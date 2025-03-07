#include <iostream>
#include <string.h>

class String {
 public:
  String(char *in);
  ~String();
  String(const String &other);
  String &operator=(const String &other);
  String(String &&other);
  String &operator=(String &&other);

  friend std::ostream &operator<<(std::ostream &os, const String &str);
 private:
  char *data_;
};

String::String(char *in) {
  data_ = new char[strlen(in) + 1];
  strcpy(data_, in);
}

String::~String() {
  delete[] data_;
}

String::String(const String &other) {
  data_ = new char[strlen(other.data_) + 1];
  strcpy(data_, other.data_);
}

String &String::operator=(const String &other) {
  if (data_ != other.data_) {
    delete[] data_;
    data_ = new char[strlen(other.data_) + 1];
    strcpy(data_, other.data_);
  }
  return *this;
}

String::String(String &&other) {
  data_ = other.data_;
  other.data_ = nullptr;
}

String &String::operator=(String &&other) {
  if (data_ != other.data_) {
    delete[] data_;
    data_ = other.data_;
    other.data_ = nullptr;
  }
  return *this;
}

std::ostream &operator<<(std::ostream &os, const String &str) {
  os << str.data_;
  return os;
}

int main() {
  String s1("Vidge Is Unique!!");
  std::cout << s1 << std::endl;

  String s2 = s1;
  std::cout << s2 << std::endl;

  String s3 = std::move(s1);
  std::cout << s3 << std::endl;
}
