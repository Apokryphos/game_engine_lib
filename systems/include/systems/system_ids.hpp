#pragma once

#include "common/system_id.hpp"

namespace systems
{
static const common::SystemId SYSTEM_ID_UNASSIGNED = 0;
static const common::SystemId SYSTEM_ID_CONFIG     = 1;
static const common::SystemId SYSTEM_ID_PROFILE    = 2;
static const common::SystemId SYSTEM_ID_NAME       = 3;

static const common::SystemId SYSTEM_ID_LAST = SYSTEM_ID_NAME;
}
