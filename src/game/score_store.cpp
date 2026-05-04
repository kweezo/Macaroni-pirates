#include "game/score_store.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

static void copyAsciiName(char *dest, size_t destCap, const char *src) {
  dest[0] = '\0';
  if (!src || destCap == 0)
    return;
  size_t w = 0;
  for (size_t i = 0; src[i] && w + 1 < destCap; ++i) {
    const unsigned char c = (unsigned char)src[i];
    if (c < 32 || c > 126)
      continue;
    dest[w++] = (char)c;
  }
  dest[w] = '\0';
}

struct DemoRow {
  const char *name;
  int score;
};

static const DemoRow kAchievableDemo[] = {
    {"Deckhand Dan", 55},
    {"Parrot", 120},
    {"Stowaway", 195},
    {"Old Salt", 280},
    {"Mackerel Pete", 380},
};

ScoreStore::ScoreStore() = default;

void ScoreStore::seedAchievableDummyLocked() {
  const size_t n =
      std::min(sizeof kAchievableDemo / sizeof kAchievableDemo[0],
               (size_t)SCORE_STORE_MAX_SCORES);
  entryCount = n;
  for (size_t i = 0; i < n; ++i) {
    entries[i] = {};
    std::strncpy(entries[i].name, kAchievableDemo[i].name,
                 sizeof entries[i].name - 1);
    entries[i].name[sizeof entries[i].name - 1] = '\0';
    entries[i].score = kAchievableDemo[i].score;
  }
  sortEntries();
  saveToFile();
}

void ScoreStore::reloadFromFile() {
  std::lock_guard lock(storeMutex);
  entryCount = 0;
  entries = {};

  std::ifstream in(filePath);
  if (!in) {
    seedAchievableDummyLocked();
    return;
  }

  std::string line;
  while (entryCount < SCORE_STORE_MAX_SCORES &&
         std::getline(in, line)) {
    const size_t sepPos = line.find('|');
    if (sepPos == std::string::npos)
      continue;
    line[sepPos] = '\0';
    const int sc = std::atoi(line.c_str() + sepPos + 1);
    copyAsciiName(entries[entryCount].name, sizeof entries[entryCount].name,
                  line.c_str());
    if (!entries[entryCount].name[0])
      continue;
    entries[entryCount].score = sc;
    ++entryCount;
  }
  sortEntries();

  if (entryCount == 0)
    seedAchievableDummyLocked();
}

void ScoreStore::sortEntries() {
  std::sort(entries.begin(), entries.begin() + entryCount,
            [](const Entry &a, const Entry &b) { return a.score > b.score; });
}

void ScoreStore::saveToFile() {
  std::ofstream out(filePath);
  if (!out)
    return;
  for (size_t i = 0; i < entryCount; ++i) {
    out << entries[i].name << '|' << entries[i].score << '\n';
  }
}

void ScoreStore::addEntry(const char *nameAscii, int score) {
  if (!nameAscii || !nameAscii[0])
    return;
  std::lock_guard lock(storeMutex);
  Entry newEntry{};
  copyAsciiName(newEntry.name, sizeof newEntry.name, nameAscii);
  if (!newEntry.name[0])
    return;
  newEntry.score = score;

  if (entryCount < SCORE_STORE_MAX_SCORES) {
    entries[entryCount] = newEntry;
    ++entryCount;
  } else {
    sortEntries();
    if (newEntry.score > entries[SCORE_STORE_MAX_SCORES - 1].score)
      entries[SCORE_STORE_MAX_SCORES - 1] = newEntry;
  }
  sortEntries();
  saveToFile();
}

void ScoreStore::topForDisplay(
    Entry (&out)[SCORE_STORE_LEADERBOARD_DISPLAY_SLOTS]) const {
  std::lock_guard lock(storeMutex);
  for (size_t i = 0; i < SCORE_STORE_LEADERBOARD_DISPLAY_SLOTS; ++i) {
    out[i] = {};
    if (i < entryCount)
      out[i] = entries[i];
  }
}
