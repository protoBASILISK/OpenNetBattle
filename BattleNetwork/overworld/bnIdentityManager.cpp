#include "bnIdentityManager.h"

#include "bnServerAssetManager.h"
#include "../bnFileUtil.h"
#include <Poco/RandomStream.h>
#include <filesystem>

constexpr std::string_view IDENTITY_FOLDER = "identity";

namespace Overworld {
  IdentityManager::IdentityManager(const std::string& address, uint16_t port)
  {
    path = std::string(IDENTITY_FOLDER) + "/" + URIEncode(address + "_p" + std::to_string(port));
  }

  std::string IdentityManager::GetIdentity() {
    if (!identity.empty()) {
      return identity;
    }

    identity = FileUtil::Read(path);

    if (identity.empty()) {
      // make sure the identity folder exists
      std::filesystem::create_directories(IDENTITY_FOLDER);

      char buffer[255];

      Poco::RandomBuf randomBuf;
      int read = randomBuf.readFromDevice(buffer, 255);

      identity = std::string(buffer, read);

      auto writeStream = FileUtil::WriteStream(path);
      writeStream << identity;
    }

    return identity;
  }
}
