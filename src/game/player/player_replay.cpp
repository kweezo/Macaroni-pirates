#include "game/player/player_replay.hpp"

#include "macros.hpp"

void PlayerReplay::closeRecordingFile() {
  std::lock_guard lock(recordingFileMutex);
  if (recordingFile) {
    fclose(recordingFile);
    recordingFile = nullptr;
  }
}

void PlayerReplay::flushRecording() { closeRecordingFile(); }

void PlayerReplay::appendIfPlaying(uint64_t relNs, float x, float y) {
  if (replayActive.load())
    return;
  const PlayerMotionSample sample{relNs, x, y};
  std::lock_guard lock(recordingFileMutex);
  if (!recordingFile)
    return;
  fwrite(&sample, sizeof sample, 1, recordingFile);
}

void PlayerReplay::finishReplay() {
  replayActive.store(false);
  samples.clear();
  replayIndex = 0;
}

bool PlayerReplay::peekReplayStart(float &ox, float &oy) {
  if (!replayActive.load() || samples.empty())
    return false;
  ox = samples[0].x;
  oy = samples[0].y;
  return true;
}

void PlayerReplay::syncReplay(float &px, float &py) {
  if (!replayActive.load() || samples.empty())
    return;

  const uint64_t rel = static_cast<uint64_t>(NANOS) - replayT0;
  while (replayIndex + 1 < samples.size() &&
         samples[replayIndex + 1].tNs <= rel)
    ++replayIndex;

  const PlayerMotionSample &cur = samples[replayIndex];
  px = cur.x;
  py = cur.y;

  const PlayerMotionSample &last = samples.back();
  if (replayIndex + 1 >= samples.size() && rel >= last.tNs)
    finishReplay();
}

void PlayerReplay::beginRound(bool replayFromMenu) {
  flushRecording();
  samples.clear();
  replayActive.store(false);
  replayIndex = 0;

  if (replayFromMenu) {
    FILE *f = fopen(tapePath, "rb");
    if (f && fseek(f, 0, SEEK_END) == 0) {
      const long bytes = ftell(f);
      const long sampleBytes = (long)sizeof(PlayerMotionSample);
      if (bytes > 0 && bytes % sampleBytes == 0) {
        size_t n = (size_t)(bytes / sampleBytes);
        if (n > maxReplaySamples)
          n = maxReplaySamples;
        rewind(f);
        samples.resize(n);
        if (n > 0 &&
            fread(samples.data(), sizeof(PlayerMotionSample), n, f) == n) {
          replayActive.store(true);
        } else {
          samples.clear();
        }
      }
    }
    if (f)
      fclose(f);
  } else {
    std::lock_guard lock(recordingFileMutex);
    recordingFile = fopen(tapePath, "wb");
  }

  recordingT0 = static_cast<uint64_t>(NANOS);
  replayT0 = recordingT0;
}
