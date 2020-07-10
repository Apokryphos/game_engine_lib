#pragma once

#include "engine/base_systems/base_system_ids.hpp"

namespace systems
{
const common::SystemId _ID = engine::BASE_SYSTEM_ID_LAST + 1;

static inline const common::SystemId SYSTEM_ID_NAME      = _ID + 1;
static inline const common::SystemId SYSTEM_ID_CAMERA    = _ID + 2;
static inline const common::SystemId SYSTEM_ID_MODEL     = _ID + 3;
static inline const common::SystemId SYSTEM_ID_POSITION  = _ID + 4;

static inline const common::SystemId SYSTEM_ID_LAST = SYSTEM_ID_POSITION;
}
