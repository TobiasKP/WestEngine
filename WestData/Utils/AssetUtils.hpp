#pragma once

#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace AssetUtils
{

static std::string generateGUID(const std::string& path)
{
  std::vector<std::uint32_t> seed;
  for (char c : path)
  {
    seed.push_back(static_cast<std::uint32_t>(c));
  }
  std::seed_seq seq(std::seed_seq(seed.begin(), seed.end()));
  std::mt19937_64 gen(seq);
  std::uniform_int_distribution<> dis(0, 15);
  std::uniform_int_distribution<> dis2(8, 11);

  std::stringstream ss;
  std::uint32_t i;
  ss << std::hex;
  for (i = 0; i < 8; i++)
  {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 4; i++)
  {
    ss << dis(gen);
  }
  ss << "-4";
  for (i = 0; i < 3; i++)
  {
    ss << dis(gen);
  }
  ss << "-" << dis2(gen);
  for (i = 0; i < 3; i++)
  {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 12; i++)
  {
    ss << dis(gen);
  }
  return ss.str();
};


};  // namespace AssetUtils
