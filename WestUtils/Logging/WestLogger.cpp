#include "../Include/WestLogger.h"

#include "../Include/TimeUtils.hpp"

#include <iostream>

WestLogger& WestLogger::getLoggerInstance()
{
  static WestLogger instance;
  return instance;
}

WestLogger::WestLogger()
{
  std::string currentDate = TimeUtils::getCurrentTimeAsDate() + ".log";
  _logFile.open(std::string(INFO_FILE_NAME) + currentDate, std::ios::out | std::ios::app);
  _errorFile.open(std::string(ERROR_FILE_NAME) + currentDate, std::ios::out | std::ios::app);
  _cycleFile.open(std::string(CYCLE_FILE_NAME) + currentDate, std::ios::out);
  _cycleLength = 100;

  if (!_logFile.is_open() || !_logFile.good())
  {
    _logFile.close();
    std::cerr << "Error opening log file - badbit|failbit|eofbit " << _logFile.bad() << _logFile.fail()
              << _logFile.eof() << std::endl;
  }
  if (!_errorFile.is_open() || !_errorFile.good())
  {
    _errorFile.close();
    std::cerr << "Error opening error log file - badbit|failbit|eofbit " << _errorFile.bad() << _errorFile.fail()
              << _errorFile.eof() << std::endl;
  }
  if (!_cycleFile.is_open() || !_cycleFile.good())
  {
    _cycleFile.close();
    std::cerr << "Error opening cycling log file - badbit|failbit|eofbit " << _cycleFile.bad() << _cycleFile.fail()
              << _cycleFile.eof() << std::endl;
  }

  _th = std::thread(&WestLogger::workerThread, this);
}

WestLogger::~WestLogger()
{
  joinThreads();
}

void WestLogger::closeFileStreams()
{
  joinThreads();
  _cycleFile.close();
  _logFile.close();
  _errorFile.close();
}

void WestLogger::joinThreads()
{
  bool expected = false;
  if (_stopWorker.compare_exchange_strong(expected, true))
  {
    _cv.notify_all();
    if (_th.joinable())
    {
      _th.join();
    }
  }
}

void WestLogger::log(const Level level, const std::string message)
{
  std::unique_lock<std::mutex> lock(_mutex);
  Message* m = new Message(level, message);
  _q.push(*m);
  lock.unlock();
  _cv.notify_one();
}

void WestLogger::workerThread()
{
  std::vector<Message> bulk;
  bulk.reserve(256);

  while (!(_stopWorker.load() && _q.empty()))
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [&] { return _q.size() < BULK_SIZE || _stopWorker.load(); });

    while (!_q.empty() && bulk.size() < 512)
    {
      bulk.push_back(std::move(_q.front()));
      _q.pop();
    }
    lock.unlock();

    std::ofstream* out = nullptr;
    for (Message current : bulk)
    {
      switch (current.mode)
      {
        case Level::Info:
          out = &_logFile;
          break;
        case Level::Error:
          out = &_errorFile;
          break;
        case Level::Cycle:
          out = &_cycleFile;
          break;
      }
      if (out && out->is_open())
      {
        (*out) << current.payload;
      }
    }
    _logFile.flush();
    _errorFile.flush();
    _cycleFile.flush();
    bulk.clear();
  }
}

std::uint8_t WestLogger::getCycleLength()
{
  std::uint8_t current = _cycleLength.load(std::memory_order_relaxed);
  if (current == 0)
  {
    _cycleLength.store(CYCLE_LENGTH, std::memory_order_relaxed);
    return 0;
  }
  return _cycleLength.fetch_sub(1, std::memory_order_relaxed);
}
