#include <iostream>
#include <string>

inline std::string readString(std::iostream& stream) {
  uint32_t len;
  char *blob;
  std::string result;

  stream.read((char*)&len, 4);
  blob = new char[len+1];
  blob[len] = '\0';
  stream.read(blob, len);
  result = blob;
  delete[] blob;
  return result;
}

inline void writeString(std::iostream& stream, const std::string& str) {
  uint32_t len = str.size();
  stream.write((char*)&len, 4);
  stream.write(str.c_str(), len);
}
