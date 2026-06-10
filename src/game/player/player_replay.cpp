#include "game/player/player_replay.hpp"

#include "macros.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace {

static bool parseMotionLine(const std::string &line, PlayerMotionSample &out) {
  if (line.empty() || line[0] == '#')
    return false;
  std::istringstream iss(line);
  uint64_t t = 0;
  float x = 0.f;
  float y = 0.f;
  if (!(iss >> t >> x >> y))
    return false;
  out.tNs = t;
  out.x = x;
  out.y = y;
  return true;
}

} // namespace

void PlayerReplay::closeRecordingFile() {
  std::lock_guard lock(recordingFileMutex);
  if (recordingOut.is_open())
    recordingOut.close();
}

void PlayerReplay::flushRecording() { closeRecordingFile(); }

void PlayerReplay::appendIfPlaying(uint64_t relNs, float x, float y) {
  if (replayActive)
    return;
  std::lock_guard lock(recordingFileMutex);
  if (!recordingOut.is_open())
    return;
  recordingOut << relNs << ' ' << x << ' ' << y << '\n';
}

void PlayerReplay::clearPlaybackState() {
  sampleCount = 0;
  replayActive = false;
  replayIndex = 0;
}

void PlayerReplay::finishReplay() { clearPlaybackState(); }

void PlayerReplay::tryLoadReplayTape() {
  std::ifstream in(tapePath);
  if (!in)
    return;

  sampleCount = 0;
  std::string line;
  while (sampleCount < maxReplaySamples && std::getline(in, line)) {
    PlayerMotionSample sample{};
    if (!parseMotionLine(line, sample))
      continue;
    samples[sampleCount++] = sample;
  }

  if (sampleCount > 0)
    replayActive = true;
}

void PlayerReplay::openRecordingTapeTruncated() {
  std::lock_guard lock(recordingFileMutex);
  if (recordingOut.is_open())
    recordingOut.close();
  recordingOut.clear();
  recordingOut.open(tapePath, std::ios::trunc);
  recordingOut << "# t_ns x y\n";
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
