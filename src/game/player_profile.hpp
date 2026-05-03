#pragma once

#include <cstring>

class PlayerProfile {
public:
  void setFromUtf8(const char *utf8) {
    if (!utf8) {
      profileName[0] = '\0';
      return;
    }
    std::strncpy(profileName, utf8, sizeof profileName - 1);
    profileName[sizeof profileName - 1] = '\0';
  }

  const char *nameUtf8() const { return profileName; }

private:
  char profileName[64]{};
};
