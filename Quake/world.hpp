/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2010-2014 QuakeSpasm developers
Copyright (C) 2020-2020 Vittorio Romeo

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef _QUAKE_WORLD_H
#define _QUAKE_WORLD_H

#include "quakeglm.hpp"

typedef struct
{
    glm::vec3 normal;
    float dist;
} plane_t;

struct trace_t
{
    bool allsolid;   // if true, plane is not valid
    bool startsolid; // if true, the initial point was in a solid area
    bool inopen, inwater;
    float fraction;   // time completed, 1.0 = didn't hit anything
    glm::vec3 endpos; // final position
    plane_t plane;    // surface normal at impact
    edict_t* ent;     // entity the surface is on
};


#define MOVE_NORMAL 0
#define MOVE_NOMONSTERS 1
#define MOVE_MISSILE 2


void SV_ClearWorld(void);
// called after the world model has been loaded, before linking any entities

void SV_UnlinkEdict(edict_t* ent);
// call before removing an entity, and before trying to move one,
// so it doesn't clip against itself
// flags ent->v.modified

void SV_LinkEdict(edict_t* ent, bool touch_triggers);
// Needs to be called any time an entity changes origin, mins, maxs, or solid
// flags ent->v.modified
// sets ent->v.absmin and ent->v.absmax
// if touch_triggers, calls prog functions for the intersected triggers

int SV_PointContents(const glm::vec3& p);
int SV_TruePointContents(const glm::vec3& p);
// returns the CONTENTS_* value from the world at the given point.
// does not check any entities at all
// the non-true version remaps the water current contents to content_water

edict_t* SV_TestEntityPositionCustomOrigin(edict_t* ent, const glm::vec3& xOrigin);
edict_t* SV_TestEntityPosition(edict_t* ent);

trace_t SV_Move(const glm::vec3& start, const glm::vec3& mins,
    const glm::vec3& maxs, const glm::vec3& end, const int type,
    edict_t* const passedict);
// mins and maxs are relative

trace_t SV_MoveTrace(const glm::vec3& start, const glm::vec3& end,
    const int type, edict_t* const passedict);

// if the entire move stays in a solid volume, trace.allsolid will be set

// if the starting point is in a solid, it will be allowed to move out
// to an open area

// nomonsters is used for line of sight or edge testing, where mosnters
// shouldn't be considered solid objects

// passedict is explicitly excluded from clipping checks (normally nullptr)
bool SV_RecursiveHullCheck(hull_t* hull, int num, float p1f, float p2f,
    const glm::vec3& p1, const glm::vec3& p2, trace_t* trace);

#endif /* _QUAKE_WORLD_H */
