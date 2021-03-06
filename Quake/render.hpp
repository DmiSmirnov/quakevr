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

#ifndef _QUAKE_RENDER_H
#define _QUAKE_RENDER_H

#include "quakeglm.hpp"

// refresh.h -- public interface to refresh functions

#define MAXCLIPPLANES 11

#define TOP_RANGE 16 // soldier uniform colors
#define BOTTOM_RANGE 96

//=============================================================================

typedef struct efrag_s
{
    struct efrag_s* leafnext;
    struct entity_t* entity;
} efrag_t;

// johnfitz -- for lerping
#define LERP_MOVESTEP \
    (1 << 0) // this is a MOVETYPE_STEP entity, enable movement lerp
#define LERP_RESETANIM (1 << 1) // disable anim lerping until next anim frame
#define LERP_RESETANIM2 \
    (1 << 2) // set this and previous flag to disable anim lerping for two anim
             // frames
#define LERP_RESETMOVE \
    (1 << 3) // disable movement lerping until next origin/angles change
#define LERP_FINISH \
    (1 << 4) // use lerpfinish time from server update instead of assuming
             // interval of 0.1
// johnfitz

struct entity_t
{
    bool forcelink; // model changed

    int update_type;

    entity_state_t baseline; // to fill in defaults in updates

    double msgtime;           // time of last update
    glm::vec3 msg_origins[2]; // last two updates (0 is newest)
    glm::vec3 origin;
    glm::vec3 msg_angles[2]; // last two updates (0 is newest)
    glm::vec3 angles;
    qmodel_t* model;       // nullptr = no model
    struct efrag_s* efrag; // linked list of efrags
    int frame;
    float syncbase; // for client-side animations
    byte* colormap;
    int effects;  // light, particles, etc
    int skinnum;  // for Alias models
    int visframe; // last frame this entity was
                  //  found in an active leaf

    int dlightframe; // dynamic lighting
    int dlightbits;

    // FIXME: could turn these into a union
    int trivial_accept;
    struct mnode_s* topnode; // for bmodels, first world node
                             //  that splits bmodel, or nullptr if
                             //  not split

    byte alpha;         // johnfitz -- alpha
    byte lerpflags;     // johnfitz -- lerping
    float lerpstart;    // johnfitz -- animation lerping
    float lerptime;     // johnfitz -- animation lerping
    float lerpfinish;   // johnfitz -- lerping -- server sent us a more accurate
                        // interval, use it instead of 0.1
    short previouspose; // johnfitz -- animation lerping
    short currentpose;  // johnfitz -- animation lerping
    //	short					futurepose;		//johnfitz -- animation lerping
    float movelerpstart;      // johnfitz -- transform lerping
    glm::vec3 previousorigin; // johnfitz -- transform lerping
    glm::vec3 currentorigin;  // johnfitz -- transform lerping
    glm::vec3 previousangles; // johnfitz -- transform lerping
    glm::vec3 currentangles;  // johnfitz -- transform lerping

    bool horizflip; // VR: horizontal flip

    // VR: per-instance scaling
    glm::vec3 msg_scales[2]; // last two updates (0 is newest)
    glm::vec3 scale;
    glm::vec3 scale_origin;

    bool hidden; // TODO VR: (P1) hack
};

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct
{
    vrect_t vrect;               // subwindow in video for refresh
                                 // FIXME: not need vrect next field here?
    vrect_t aliasvrect;          // scaled Alias version
    int vrectright, vrectbottom; // right & bottom screen coords
    int aliasvrectright, aliasvrectbottom; // scaled Alias versions
    float vrectrightedge;           // rightmost right edge we care about,
                                    //  for use in edge list
    float fvrectx, fvrecty;         // for floating-point compares
    float fvrectx_adj, fvrecty_adj; // left and top edges, for clamping
    int vrect_x_adj_shift20;        // (vrect.x + 0.5 - epsilon) << 20
    int vrectright_adj_shift20;     // (vrectright + 0.5 - epsilon) << 20
    float fvrectright_adj, fvrectbottom_adj;
    // right and bottom edges, for clamping
    float fvrectright;           // rightmost edge, for Alias clamping
    float fvrectbottom;          // bottommost edge, for Alias clamping
    float horizontalFieldOfView; // at Z = 1.0, this many X is visible
                                 // 2.0 = 90 degrees
    float xOrigin;               // should probably allways be 0.5
    float yOrigin;               // between be around 0.3 to 0.5

    glm::vec3 vieworg;
    glm::vec3 viewangles;
    glm::vec3 aimangles;

    float fov_x, fov_y;

    int ambientlight;
} refdef_t;


//
// refresh
//
extern int reinit_surfcache;


extern refdef_t r_refdef;
extern glm::vec3 r_origin, vpn, vright, vup;


void R_Init(void);
void R_InitTextures(void);
void R_InitEfrags(void);
void R_RenderView(void); // must set r_refdef first
void R_RenderScene(void);
void R_DrawViewModel(entity_t* viewent);
void R_ViewChanged(vrect_t* pvrect, int lineadj, float aspect);
// called whenever r_refdef or vid change
// void R_InitSky (struct texture_s *mt);	// called at level load

void R_CheckEfrags(void); // johnfitz
void R_AddEfrags(entity_t* ent);

void R_NewMap(void);


void R_ParseParticleEffect(void);
void R_ParseParticle2Effect(void);
void R_RunParticle2Effect(
    const glm::vec3& org, const glm::vec3& dir, int preset, int count);
void R_RunParticleEffect_BulletPuff(
    const glm::vec3& org, const glm::vec3& dir, int color, int count);
void R_RocketTrail(glm::vec3 start, const glm::vec3& end, int type);
void R_EntityParticles(entity_t* ent);
void R_ParticleExplosion(const glm::vec3& org);
void R_ParticleExplosion2(
    const glm::vec3& org, int colorStart, int colorLength);
void R_LavaSplash(const glm::vec3& org);
void R_TeleportSplash(const glm::vec3& org);

void R_PushDlights(void);

struct aliashdr_t;
struct lerpdata_t;

void R_SetupAliasFrame(
    entity_t* e, aliashdr_t* paliashdr, int frame, lerpdata_t* lerpdata);

void R_SetupEntityTransform(entity_t* e, lerpdata_t* lerpdata);


//
// surface cache related
//
extern int reinit_surfcache; // if 1, surface cache is currently empty and
extern bool r_cache_thrash;  // set if thrashing the surface cache

int D_SurfaceCacheForRes(int width, int height);
void D_FlushCaches(void);
void D_DeleteSurfaceCache(void);
void D_InitCaches(void* buffer, int size);
void R_SetVrect(vrect_t* pvrect, vrect_t* pvrectin, int lineadj);

#endif /* _QUAKE_RENDER_H */
