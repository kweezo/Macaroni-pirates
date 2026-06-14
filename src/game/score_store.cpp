#include "game/score_store.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>

static constexpr size_t kNameCap = sizeof(ScoreStore::Entry::name);
// Record layout: kNameCap bytes (null-padded name) + 4 bytes (int32_t score)
static constexpr size_t kRecordSize = kNameCap + sizeof(int32_t);

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
    copyAsciiName(entries[i].name, kNameCap, kAchievableDemo[i].name);
    entries[i].score = kAchievableDemo[i].score;
  }
  sortEntries();
  saveToFile();
}

void ScoreStore::reloadFromFile() {
  std::lock_guard lock(storeMutex);
  entryCount = 0;
  entries = {};

  std::ifstream in(filePath, std::ios::binary);
  if (!in) {
    seedAchievableDummyLocked();
    return;
  }

  char buf[kRecordSize];
  while (entryCount < SCORE_STORE_MAX_SCORES &&
         in.read(buf, kRecordSize)) {
    buf[kNameCap - 1] = '\0';
    if (!buf[0])
      continue;
    std::memcpy(entries[entryCount].name, buf, kNameCap);
    int32_t score;
    std::memcpy(&score, buf + kNameCap, sizeof(int32_t));
    entries[entryCount].score = static_cast<int>(score);
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
  std::ofstream out(filePath, std::ios::binary | std::ios::trunc);
  if (!out)
    return;

  char buf[kRecordSize];
  for (size_t i = 0; i < entryCount; ++i) {
    std::memset(buf, 0, kRecordSize);
    std::memcpy(buf, entries[i].name, kNameCap);
    const int32_t score = static_cast<int32_t>(entries[i].score);
    std::memcpy(buf + kNameCap, &score, sizeof(int32_t));
    out.write(buf, kRecordSize);
  }
}

void ScoreStore::addEntry(const char *nameAscii, int score) {
  if (!nameAscii || !nameAscii[0])
    return;
  std::lock_guard lock(storeMutex);
  Entry newEntry{};
  copyAsciiName(newEntry.name, kNameCap, nameAscii);
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