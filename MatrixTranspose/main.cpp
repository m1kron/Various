#include <assert.h>

#include <cstring>
#include <iostream>

#include "matrix2d.h"
#include "randomGenerator.h"
#include "timer.h"

using namespace std;

namespace alg {
// Basic, cache inefficient algorithm.
static bool Transpose_Simple(const Matrix2D& a, Matrix2D& out) {
  if (a.GetYSize() != out.GetXSize() || a.GetXSize() != out.GetYSize())
    return false;

  for (int y = 0; y < a.GetYSize(); ++y) {
    for (int x = 0; x < a.GetXSize(); ++x) {
      out.At(x, y) = a.At(y, x);
    }
  }

  return true;
}

// Blocking implementation.
static bool Transpose_Blocking(const Matrix2D& a, Matrix2D& out) {
  if (a.GetYSize() != out.GetXSize() || a.GetXSize() != out.GetYSize())
    return false;

  // Fine-tune this params:
  constexpr int BLOCK_HEIGHT = 32;
  constexpr int BLOCK_WIDTH = 32;

  for (int blockYStartIdx = 0; blockYStartIdx < a.GetYSize();
       blockYStartIdx += BLOCK_HEIGHT) {
    for (int blockXStartIdx = 0; blockXStartIdx < a.GetXSize();
         blockXStartIdx += BLOCK_WIDTH) {
      // Transpose block:
      const int ySize = std::min(a.GetYSize(), blockYStartIdx + BLOCK_HEIGHT);
      const int xSize = std::min(a.GetXSize(), blockXStartIdx + BLOCK_WIDTH);
      for (int y = blockYStartIdx; y < ySize; ++y) {
        for (int x = blockXStartIdx; x < xSize; ++x) {
          out.At(x, y) = a.At(y, x);
        }
      }
    }
  }

  return true;
}

// Private implementation of cache oblivious algorithm.
static void Transpose_CacheObliviousImpl(const Matrix2D& a, Matrix2D& out,
                                         int blockYStartIdx, int blockXStartIdx,
                                         int blockYSize, int blockXSize) {
  const int MAX_Y_BLOCKSIZE = 16;
  const int MAX_X_BLOCKSIZE = 16;
  if (blockYSize <= MAX_Y_BLOCKSIZE && blockXSize <= MAX_X_BLOCKSIZE) {
    const int ySize = std::min(a.GetYSize(), blockYStartIdx + blockYSize);
    const int xSize = std::min(a.GetXSize(), blockXStartIdx + blockXSize);
    for (int y = blockYStartIdx; y < ySize; ++y) {
      for (int x = blockXStartIdx; x < xSize; ++x) {
        out.At(x, y) = a.At(y, x);
      }
    }
  } else {
    // Recurse:
    const int newBlockYSize = blockYSize / 2;
    const int newBlockXSize = blockXSize / 2;

    Transpose_CacheObliviousImpl(a, out, blockYStartIdx, blockXStartIdx,
                                 newBlockYSize, newBlockXSize);
    Transpose_CacheObliviousImpl(a, out, blockYStartIdx,
                                 blockXStartIdx + newBlockXSize, newBlockYSize,
                                 blockXSize - newBlockXSize);
    Transpose_CacheObliviousImpl(a, out, blockYStartIdx + newBlockYSize,
                                 blockXStartIdx, blockYSize - newBlockYSize,
                                 newBlockXSize);
    Transpose_CacheObliviousImpl(
        a, out, blockYStartIdx + newBlockYSize, blockXStartIdx + newBlockXSize,
        blockYSize - newBlockYSize, blockXSize - newBlockXSize);
  }
}

// Cache oblivious implementation.
static bool Transpose_CacheOblivious(const Matrix2D& a, Matrix2D& out) {
  if (a.GetYSize() != out.GetXSize() || a.GetXSize() != out.GetYSize())
    return false;

  Transpose_CacheObliviousImpl(a, out, 0, 0, a.GetYSize(), a.GetXSize());

  return true;
}
}  // namespace alg

namespace helpers {
// Returns true if matrix transposeA is transpose of matrix a.
static bool IsTranspose(const Matrix2D& a, const Matrix2D& transposeA) {
  if (a.GetYSize() != transposeA.GetXSize() ||
      a.GetXSize() != transposeA.GetYSize())
    return false;
  for (int y = 0; y < a.GetYSize(); ++y) {
    for (int x = 0; x < a.GetXSize(); ++x) {
      if (a.At(y, x) != transposeA.At(x, y)) return false;
    }
  }

  return true;
}

// Performs benchmark over given function.
template <typename TFunction, typename... TArgs>
void PerformBenchmark(const TFunction function, const char* funcName,
                      int warmupRuns, int benchmarkRuns, TArgs&... args) {
  tools::Timer timer;
  for (int i = 0; i < warmupRuns; ++i) const bool success = function(args...);
  timer.Start();
  for (int i = 0; i < benchmarkRuns; ++i)
    const bool success = function(args...);
  timer.Stop();
  cout << "Transpose with '" << funcName << "' was done in avg "
       << timer.GetElapsedTimeInMiliseconds() / benchmarkRuns << " ms!" << endl;
  cout << "Is transpose valid: " << IsTranspose(args...) << endl;
}

#define PERFORM_BENCHMARK(function, warmupRuns, benchamrkRuns, ...)         \
  helpers::PerformBenchmark(function, #function, warmupRuns, benchamrkRuns, \
                            __VA_ARGS__);

// Prints matrix.
static void Print(const Matrix2D& matrix) {
  for (int y = 0; y < matrix.GetYSize(); ++y) {
    for (int x = 0; x < matrix.GetXSize(); ++x) {
      cout << matrix.At(y, x) << " ";
    }
    cout << endl;
  }
  cout << endl;
}
}  // namespace helpers

////////////////////////////////////////////////////////////////
int main() {
  const int HEIGTH = 7000;
  const int WIDTH = 6000;

  Matrix2D a(HEIGTH, WIDTH, tools::GenerateRandomInt(WIDTH * HEIGTH));
  Matrix2D transposed(WIDTH, HEIGTH);

  const Matrix2D& matrix = a;
  Matrix2D& out = transposed;

  PERFORM_BENCHMARK(alg::Transpose_CacheOblivious, 5, 5, matrix, out);
  PERFORM_BENCHMARK(alg::Transpose_Blocking, 5, 5, matrix, out);
  PERFORM_BENCHMARK(alg::Transpose_Simple, 5, 5, matrix, out);

  // helpers::Print(matrix);
  // helpers::Print(out);

  return 0;
}