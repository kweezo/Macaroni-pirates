#include "game/player/player_replay.hpp"

#include "macros.hpp"

void PlayerReplay::closeRecordingFile() {
  std::lock_guard lock(recordingFileMutex);
  if (recordingOut.is_open())
    recordingOut.close();
}

void PlayerReplay::flushRecording() { closeRecordingFile(); }

void PlayerReplay::appendIfPlaying(uint64_t relNs, float x, float y) {
  if (replayActive)
    return;
  const PlayerMotionSample sample{relNs, x, y};
  std::lock_guard lock(recordingFileMutex);
  if (!recordingOut.is_open())
    return;
  recordingOut.write(reinterpret_cast<const char *>(&sample), sizeof sample);
}

void PlayerReplay::clearPlaybackState() {
  sampleCount = 0;
  replayActive = false;
  replayIndex = 0;
}

void PlayerReplay::finishReplay() { clearPlaybackState(); }

void PlayerReplay::tryLoadReplayTape() {
  std::ifstream in(tapePath, std::ios::binary | std::ios::ate);
  if (!in)
    return;

  const std::streamoff endOff = in.tellg();
  if (endOff <= 0)
    return;

  const uint64_t bytes = static_cast<uint64_t>(endOff);
  const size_t sampleSz = sizeof(PlayerMotionSample);
  if (sampleSz == 0 || bytes % sampleSz != 0)
    return;

  size_t n = static_cast<size_t>(bytes / sampleSz);
  if (n > maxReplaySamples)
    n = maxReplaySamples;

  in.seekg(0);
  const std::streamsize readSz = static_cast<std::streamsize>(n * sampleSz);
  if (n == 0 || !in.read(reinterpret_cast<char *>(samples.data()), readSz) ||
      in.gcount() != readSz)
    return;

  sampleCount = n;
  replayActive = true;
}

void PlayerReplay::openRecordingTapeTruncated() {
  std::lock_guard lock(recordingFileMutex);
  if (recordingOut.is_open())
    recordingOut.close();
  recordingOut.clear();
  recordingOut.open(tapePath, std::ios::binary | std::ios::trunc);
}

bool PlayerReplay::peekReplayStart(float &ox, float &oy) {
  if (!replayActive || sampleCount == 0)
    return false;
  ox = samples[0].x;
  oy = samples[0].y;
  return true;
}

void PlayerReplay::syncReplay(float &px, float &py) {
  if (!replayActive || sampleCount == 0)
    return;

  const uint64_t rel = static_cast<uint64_t>(NANOS) - replayT0;
  while (replayIndex + 1 < sampleCount &&
         samples[replayIndex + 1].tNs <= rel)
    ++replayIndex;

  const PlayerMotionSample &cur = samples[replayIndex];
  px = cur.x;
  py = cur.y;

  const PlayerMotionSample &last = samples[sampleCount - 1];
  if (replayIndex + 1 >= sampleCount && rel >= last.tNs)
    finishReplay();
}

void PlayerReplay::beginRound(bool replayFromMenu) {
  flushRecording();
  clearPlaybackState();

  if (replayFromMenu)
    tryLoadReplayTape();
  else
    openRecordingTapeTruncated();

  recordingT0 = static_cast<uint64_t>(NANOS);
  replayT0 = recordingT0;
}
