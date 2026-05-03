#include "game/score_store.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

ScoreStore::ScoreStore() = default;

void ScoreStore::reloadFromFile() {
  std::lock_guard lock(storeMutex);
  entryCount = 0;
  entries = {};

  FILE *f = fopen(filePath, "r");
  if (!f)
    return;

  char line[256];
  while (entryCount < SCORE_STORE_MAX_SCORES &&
         std::fgets(line, sizeof line, f)) {
    char *sep = std::strchr(line, '|');
    if (!sep)
      continue;
    *sep = '\0';
    int sc = std::atoi(sep + 1);
    std::strncpy(entries[entryCount].name, line, sizeof entries[entryCount].name - 1);
    entries[entryCount].name[sizeof entries[entryCount].name - 1] = '\0';
    entries[entryCount].score = sc;
    ++entryCount;
  }
  fclose(f);
  sortEntries();
}

void ScoreStore::sortEntries() {
  std::sort(entries.begin(), entries.begin() + entryCount,
            [](const Entry &a, const Entry &b) { return a.score > b.score; });
}

void ScoreStore::saveToFile() {
  FILE *f = fopen(filePath, "w");
  if (!f)
    return;
  for (size_t i = 0; i < entryCount; ++i) {
    std::fprintf(f, "%s|%d\n", entries[i].name, entries[i].score);
  }
  fclose(f);
}

void ScoreStore::addEntry(const char *nameUtf8, int score) {
  if (!nameUtf8 || !nameUtf8[0])
    return;
  std::lock_guard lock(storeMutex);
  Entry newEntry{};
  std::strncpy(newEntry.name, nameUtf8, sizeof newEntry.name - 1);
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
