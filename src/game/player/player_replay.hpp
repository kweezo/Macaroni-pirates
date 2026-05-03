#pragma once

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <vector>

#include "game/player/player_motion.hpp"

class PlayerReplay {
public:
  void flushRecording();
  void appendIfPlaying(uint64_t relNs, float x, float y);
  void queueReplay(bool v) { replayRequested.store(v); }
  bool consumeReplayRequest() { return replayRequested.exchange(false); }
  bool isReplayActive() const { return replayActive.load(); }

  bool peekReplayStart(float &ox, float &oy);
  void syncReplay(float &px, float &py);

  void beginRound(bool replayFromMenu);

  uint64_t recordingTimeOrigin() const { return recordingT0; }

private:
  void finishReplay();
  void closeRecordingFile();

  static constexpr const char *tapePath = "player_last_round.mot";
  static constexpr size_t maxReplaySamples = 400000;

  FILE *recordingFile = nullptr;
  std::vector<PlayerMotionSample> samples;
  size_t replayIndex = 0;
  uint64_t recordingT0 = 0;
  uint64_t replayT0 = 0;

  std::atomic<bool> replayActive{false};
  std::atomic<bool> replayRequested{false};
  std::mutex recordingFileMutex;
};
