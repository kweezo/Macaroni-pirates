#pragma once

#include <cstring>

class PlayerProfile {
public:
  void setFromAscii(const char *ascii) {
    if (!ascii) {
      profileName[0] = '\0';
      return;
    }
    std::strncpy(profileName, ascii, sizeof profileName - 1);
    profileName[sizeof profileName - 1] = '\0';
  }

  const char *nameAscii() const { return profileName; }

private:
  char profileName[64]{};
};
