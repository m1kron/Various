#pragma once
#include <assert.h>

#include <vector>

// Simple implementation of 2d matrix.
class Matrix2D {
 public:
  // Ctor.
  Matrix2D(int ySize, int xSize);

  // Ctor.
  Matrix2D(int ySize, int xSize, std::vector<int>&& data);

  // Getters/Setters.
  int& At(int y, int x);
  int At(int y, int x) const;

  // Returns x size.
  int GetXSize() const;

  // Retruns y size.
  int GetYSize() const;

 private:
  std::vector<int> m_data;
  int m_ySize;
  int m_xSize;
};

//////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
inline Matrix2D::Matrix2D(int ySize, int xSize)
    : m_ySize(ySize), m_xSize(xSize) {
  m_data.resize(ySize * xSize);
  for (auto& val : m_data) val = 0;
}

//////////////////////////////////////////////////////////////////////////
inline Matrix2D::Matrix2D(int ySize, int xSize, std::vector<int>&& data)
    : m_ySize(ySize), m_xSize(xSize), m_data(std::move(data)) {
  assert(m_data.size() = ySize * xSize);
}

//////////////////////////////////////////////////////////////////////////
inline int& Matrix2D::At(int y, int x) { return m_data[y * m_xSize + x]; }

//////////////////////////////////////////////////////////////////////////
inline int Matrix2D::At(int y, int x) const { return m_data[y * m_xSize + x]; }

//////////////////////////////////////////////////////////////////////////
inline int Matrix2D::GetXSize() const { return m_xSize; }

//////////////////////////////////////////////////////////////////////////
inline int Matrix2D::GetYSize() const { return m_ySize; }