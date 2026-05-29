#include "BinaryLoader.hpp"


/*BinaryLoader(WestLogger* l);
~BinaryLoader();

void init();
void shutdown();

bool exists(const std::string path);
const Model& load(const std::string path);

private:
  const Model& loadFromDisk(const std::string& path);

std::unique_ptr<Converter> _converter;
WestLogger* _logger;*/

BinaryLoader::BinaryLoader(WestLogger* l)
{
  _logger    = l;
  _converter = std::make_unique<Converter>(l);
}

BinaryLoader::~BinaryLoader() {}


void BinaryLoader::init()
{
  _converter->init();
}

void BinaryLoader::shutdown()
{
  _converter->shutdown();
}

bool BinaryLoader::exists(const std::string& modelname)
{
  return false;
}

std::optional<Model> BinaryLoader::load(const std::string& modelname)
{
  if (exists(modelname))
  {
    return loadFromDisk(modelname);
  }
  else
  {
    _converter->convertQueueElements();
    return loadFromDisk(modelname);
  }
}
