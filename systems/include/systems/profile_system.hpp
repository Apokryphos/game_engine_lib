#pragma once

#include "common/system.hpp"
#include <cassert>
#include <memory>
#include <vector>

namespace systems
{
class ProfileSystem : public common::System
{
public:
    class Profile
    {
        std::string m_name;

    public:
        Profile(const std::string& name)
        : m_name(name) {
            assert(!name.empty());
        }

        const std::string& get_name() const {
            return m_name;
        }
    };

private:
    Profile* m_active;
    std::vector<std::unique_ptr<Profile>> m_profiles;

public:
    ProfileSystem(const std::string& game_base_name);
    Profile* create_profile(const std::string& name);
    Profile* get_active_profile();
    void get_profiles(std::vector<Profile*>& profiles);
    bool has_active_profile() const;
    void load_profiles();
    bool profile_exists(const std::string& name) const;
    void set_active_profile(Profile* profile);
};
}
