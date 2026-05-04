#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>

#define SCORE_STORE_MAX_SCORES 12
#define SCORE_STORE_LEADERBOARD_DISPLAY_SLOTS 5

class ScoreStore {
public:
  struct Entry {
    char name[64]{};
    int score = 0;
  };

  ScoreStore();

  void reloadFromFile();
  void addEntry(const char *nameAscii, int score);
  void topForDisplay(Entry (&out)[SCORE_STORE_LEADERBOARD_DISPLAY_SLOTS]) const;

private:
  void sortEntries();
  void saveToFile();
  void seedAchievableDummyLocked();

  std::array<Entry, SCORE_STORE_MAX_SCORES> entries{};
  size_t entryCount = 0;
  mutable std::mutex storeMutex;
  static constexpr const char *filePath = "scores.txt";
};
