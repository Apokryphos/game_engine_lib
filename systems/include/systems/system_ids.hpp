#pragma once

#include "engine/base_systems/base_system_ids.hpp"

namespace systems
{
const common::SystemId _ID = engine::BASE_SYSTEM_ID_LAST + 1;

static inline const common::SystemId SYSTEM_ID_NAME      = _ID + 1;
static inline const common::SystemId SYSTEM_ID_BILLBOARD = _ID + 2;
static inline const common::SystemId SYSTEM_ID_CAMERA    = _ID + 3;
static inline const common::SystemId SYSTEM_ID_GLYPH     = _ID + 4;
static inline const common::SystemId SYSTEM_ID_MODEL     = _ID + 5;
static inline const common::SystemId SYSTEM_ID_MOVE      = _ID + 6;
static inline const common::SystemId SYSTEM_ID_POSITION  = _ID + 7;
static inline const common::SystemId SYSTEM_ID_SPINE     = _ID + 8;
static inline const common::SystemId SYSTEM_ID_SPRITE    = _ID + 9;

static inline const common::SystemId SYSTEM_ID_LAST = SYSTEM_ID_SPRITE;
}
