#include "common/log.hpp"
#include "systems/profile_system.hpp"
#include "systems/system_ids.hpp"

using namespace common;

namespace systems
{
using Profile = ProfileSystem::Profile;

//  ----------------------------------------------------------------------------
ProfileSystem::ProfileSystem(const std::string& game_base_name)
: System(ProfileSystem::Id, "profile_system"),
  m_active(nullptr) {
}

//  ----------------------------------------------------------------------------
Profile* ProfileSystem::create_profile(const std::string& name) {
    if (profile_exists(name)) {
        throw std::runtime_error("A profile with that name already exists.");
    }

    auto profile = std::make_unique<Profile>(name);

    Profile* profile_ptr = profile.get();

    m_profiles.push_back(std::move(profile));

    log_debug("Profile '%s' created.", profile_ptr->get_name().c_str());

    return profile_ptr;
}

//  ----------------------------------------------------------------------------
Profile* ProfileSystem::get_active_profile() {
    return m_active;
}

//  ----------------------------------------------------------------------------
void ProfileSystem::get_profiles(std::vector<Profile*>& profiles) {
    for (auto& profile : m_profiles) {
        profiles.push_back(profile.get());
    }
}

//  ----------------------------------------------------------------------------
bool ProfileSystem::has_active_profile() const {
    return m_active != nullptr;
}

//  ----------------------------------------------------------------------------
void ProfileSystem::load_profiles() {
    log_debug("Loading profiles...");

    m_active = nullptr;
    m_profiles.clear();

    //  TODO: Load profiles from filesystem
    m_profiles.push_back(std::make_unique<Profile>("Bertard"));
}

//  ----------------------------------------------------------------------------
bool ProfileSystem::profile_exists(const std::string& name) const {
    const auto& find = std::find_if(
        m_profiles.begin(),
        m_profiles.end(),
        [&name](const auto& profile) {
            return string_compare(profile->get_name(), name);
        }
    );

    return find != m_profiles.end();
}

//  ----------------------------------------------------------------------------
void ProfileSystem::set_active_profile(Profile* profile) {
    m_active = profile;
}
}
