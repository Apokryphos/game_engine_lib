#pragma once

#include "engine/systems/base_system_ids.hpp"

namespace systems
{
const common::SystemId _ID = engine::BASE_SYSTEM_ID_LAST + 1;

static inline const common::SystemId SYSTEM_ID_NAME      = _ID + 1;
static inline const common::SystemId SYSTEM_ID_POSITION  = _ID + 2;

static inline const common::SystemId SYSTEM_ID_LAST = SYSTEM_ID_POSITION;
}
