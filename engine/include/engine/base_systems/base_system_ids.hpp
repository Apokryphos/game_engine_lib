#pragma once

#include "common/system_id.hpp"

namespace engine
{
static const common::SystemId SYSTEM_ID_DEBUG_GUI  = 1;
static const common::SystemId SYSTEM_ID_CONFIG     = 2;
static const common::SystemId SYSTEM_ID_PROFILE    = 3;

static const common::SystemId BASE_SYSTEM_ID_LAST = SYSTEM_ID_PROFILE;
}
