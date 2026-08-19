#pragma once


class nocopy {
public:
  nocopy() {}
  ~nocopy() {}
  nocopy(const nocopy &) = delete;
  nocopy& operator=(const nocopy &) = delete;

};