#pragma once

#include <array>
#include <cstdint>
#include <fstream>
#include <mutex>

#include "game/player/player_motion.hpp"

class PlayerReplay {
public:
  void flushRecording();
  void appendIfPlaying(uint64_t relNs, float x, float y);
  void queueReplay(bool v) { replayRequested = v; }
  bool consumeReplayRequest() {
    const bool r = replayRequested;
    replayRequested = false;
    return r;
  }
  bool isReplayActive() const { return replayActive; }

  bool peekReplayStart(float &ox, float &oy);
  void syncReplay(float &px, float &py);

  void beginRound(bool replayFromMenu);

  uint64_t recordingTimeOrigin() const { return recordingT0; }

private:
  void finishReplay();
  void closeRecordingFile();

  void clearPlaybackState();
  void tryLoadReplayTape();
  void openRecordingTapeTruncated();

  static constexpr const char *tapePath = "player_last_round.mot";
  static constexpr size_t maxReplaySamples = 400000;

  std::ofstream recordingOut;
  std::array<PlayerMotionSample, maxReplaySamples> samples{};
  size_t sampleCount = 0;
  size_t replayIndex = 0;
  uint64_t recordingT0 = 0;
  uint64_t replayT0 = 0;

  bool replayActive = false;
  bool replayRequested = false;
  std::mutex recordingFileMutex;
};
