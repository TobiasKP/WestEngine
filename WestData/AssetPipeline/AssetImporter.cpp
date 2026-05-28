#include "AssetImporter.hpp"

AssetImporter::AssetImporter(WestLogger* l)
{
  _logger   = l;
  _queue    = std::make_shared<AssetQueue>();
  _screener = std::make_unique<AssetPathScreener>(l, handlePath); 
}
