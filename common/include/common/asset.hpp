#pragma once

#include <cstdint>
#include <string>

namespace common
{
typedef uint32_t AssetId;

//  A resource loaded from the filesystem
class Asset
{
    AssetId m_id;
    std::string m_path;

public:
    Asset(const AssetId id, const std::string& path)
    : m_id(id),
      m_path(path) {
    }

    virtual ~Asset() {
        unload();
    }

    Asset(const Asset&) = delete;
    Asset& operator=(const Asset&) = delete;

    AssetId get_id() const {
        return m_id;
    }

    const std::string& get_path() const {
        return m_path;
    }

    virtual void load() {}
    virtual void unload() {}
};
}
