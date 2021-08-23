//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
  @file Tokens.h

      This header defines the ... functionality.

  @note
*/


#ifndef Tokens_H
#define Tokens_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDataTypes.h"

namespace U3D_IDTF
{

//***************************************************************************
//  Defines
//***************************************************************************

#define F32_EXPONENTIAL

//***************************************************************************
//  Constants
//***************************************************************************

// data types
static const IFXCHAR IDTF_INT3[] = L"INT3";
static const IFXCHAR IDTF_COLOR3[] = L"COLOR3";
static const IFXCHAR IDTF_COLOR4[] = L"COLOR4";
static const IFXCHAR IDTF_POINT3[] = L"POINT3";
static const IFXCHAR IDTF_QUAT[] = L"QUAT";
static const IFXCHAR IDTF_STRING[] = L"STRING";
static const IFXCHAR IDTF_TM[] = L"TM";
static const IFXCHAR IDTF_HEX[] = L"HEX";
static const IFXCHAR IDTF_TRUE[] = L"TRUE";
static const IFXCHAR IDTF_FALSE[] = L"FALSE";

// file header
static const IFXCHAR IDTF_FORMAT[] = L"FILE_FORMAT";
static const IFXCHAR IDTF_VERSION[] = L"FORMAT_VERSION";

// scene block
static const IFXCHAR IDTF_SCENE[] = L"SCENE";
//static const IFXCHAR IDTF_SCENE_BACKGROUND_COLOR[] = L"BACKGROUND_COLOR";
//static const IFXCHAR IDTF_SCENE_AMBIENT_LIGHT[] = L"AMBIENT_LIGHT";

// node block
static const IFXCHAR IDTF_NODE[] = L"NODE";
static const IFXCHAR IDTF_NODE_NAME[] = L"NODE_NAME";
static const IFXCHAR IDTF_PARENT_LIST[] = L"PARENT_LIST";
static const IFXCHAR IDTF_PARENT_COUNT[] = L"PARENT_COUNT";
static const IFXCHAR IDTF_PARENT[] = L"PARENT";
static const IFXCHAR IDTF_PARENT_NAME[] = L"PARENT_NAME";
static const IFXCHAR IDTF_PARENT_TM[] = L"PARENT_TM";
static const IFXCHAR IDTF_NODE_RESOURCE_ID[] = L"NODE_RESOURCE_ID";

// group
static const IFXCHAR IDTF_GROUP[] = L"GROUP";

// light
static const IFXCHAR IDTF_LIGHT[] = L"LIGHT";
static const IFXCHAR IDTF_LIGHT_TYPE[] = L"LIGHT_TYPE";
static const IFXCHAR IDTF_AMBIENT_LIGHT[] = L"AMBIENT";
static const IFXCHAR IDTF_DIRECTIONAL_LIGHT[] = L"DIRECTIONAL";
static const IFXCHAR IDTF_POINT_LIGHT[] = L"POINT";
static const IFXCHAR IDTF_SPOT_LIGHT[] = L"SPOT";
static const IFXCHAR IDTF_LIGHT_COLOR[] = L"LIGHT_COLOR";
static const IFXCHAR IDTF_LIGHT_ATTENUATION[] = L"LIGHT_ATTENUATION";
static const IFXCHAR IDTF_LIGHT_SPOT_ANGLE[] = L"LIGHT_SPOT_ANGLE";
static const IFXCHAR IDTF_LIGHT_INTENSITY[] = L"LIGHT_INTENSITY";

// view
static const IFXCHAR IDTF_VIEW[] = L"VIEW";
static const IFXCHAR IDTF_VIEW_TYPE[] = L"VIEW_TYPE";
static const IFXCHAR IDTF_VIEW_DATA[] = L"VIEW_DATA";
static const IFXCHAR IDTF_ORTHO_VIEW[] = L"ORTHO";
static const IFXCHAR IDTF_PERSPECTIVE_VIEW[] = L"PERSPECTIVE";
static const IFXCHAR IDTF_VIEW_ATTRIBUTE_SCREEN_UNIT[] = L"VIEW_ATTRIBUTE_SCREEN_UNIT";
static const IFXCHAR IDTF_VIEW_UNIT_PIXEL[] = L"PIXEL";
static const IFXCHAR IDTF_VIEW_UNIT_PERCENT[] = L"PERCENT";
static const IFXCHAR IDTF_VIEW_NEAR_CLIP[] = L"VIEW_NEAR_CLIP";
static const IFXCHAR IDTF_VIEW_FAR_CLIP[] = L"VIEW_FAR_CLIP";
static const IFXCHAR IDTF_VIEW_PROJECTION[] = L"VIEW_PROJECTION";
static const IFXCHAR IDTF_VIEW_PORT_WIDTH[] = L"VIEW_PORT_WIDTH";
static const IFXCHAR IDTF_VIEW_PORT_HEIGHT[] = L"VIEW_PORT_HEIGHT";
static const IFXCHAR IDTF_VIEW_PORT_H_POSITION[] = L"VIEW_PORT_H_POSITION";
static const IFXCHAR IDTF_VIEW_PORT_V_POSITION[] = L"VIEW_PORT_V_POSITION";
static const IFXCHAR IDTF_BACKDROP_COUNT[] = L"BACKDROP_COUNT";
static const IFXCHAR IDTF_BACKDROP_LIST[] = L"BACKDROP_LIST";
static const IFXCHAR IDTF_BACKDROP[] = L"BACKDROP";
static const IFXCHAR IDTF_OVERLAY_COUNT[] = L"OVERLAY_COUNT";
static const IFXCHAR IDTF_OVERLAY_LIST[] = L"OVERLAY_LIST";
static const IFXCHAR IDTF_OVERLAY[] = L"OVERLAY";
static const IFXCHAR IDTF_TEXTURE_BLEND[] = L"TEXTURE_BLEND";
static const IFXCHAR IDTF_ROTATION[] = L"ROTATION";
static const IFXCHAR IDTF_LOCATION_X[] = L"LOCATION_X";
static const IFXCHAR IDTF_LOCATION_Y[] = L"LOCATION_Y";
static const IFXCHAR IDTF_REG_POINT_X[] = L"REG_POINT_X";
static const IFXCHAR IDTF_REG_POINT_Y[] = L"REG_POINT_Y";
static const IFXCHAR IDTF_SCALE_X[] = L"SCALE_X";
static const IFXCHAR IDTF_SCALE_Y[] = L"SCALE_Y";

// model
static const IFXCHAR IDTF_MODEL[] = L"MODEL";
static const IFXCHAR IDTF_MODEL_VISIBILITY[] = L"MODEL_VISIBILITY";

// resource list block
static const IFXCHAR IDTF_RESOURCE_LIST[] = L"RESOURCE_LIST";
static const IFXCHAR IDTF_RESOURCE_COUNT[] = L"RESOURCE_COUNT";
static const IFXCHAR IDTF_RESOURCE[] = L"RESOURCE";
static const IFXCHAR IDTF_RESOURCE_NAME[] = L"RESOURCE_NAME";

// view resource
static const IFXCHAR IDTF_VIEW_PASS_COUNT[] = L"VIEW_PASS_COUNT";
static const IFXCHAR IDTF_VIEW_BACKDROP_COUNT[] = L"VIEW_BACKDROP_COUNT";
static const IFXCHAR IDTF_VIEW_OVERLAY_COUNT[] = L"VIEW_OVERLAY_COUNT";
static const IFXCHAR IDTF_VIEW_ROOT_NODE_LIST[] = L"VIEW_ROOT_NODE_LIST";
static const IFXCHAR IDTF_ROOT_NODE[] = L"ROOT_NODE";
static const IFXCHAR IDTF_ROOT_NODE_NAME[] = L"ROOT_NODE_NAME";

// model resource
static const IFXCHAR IDTF_MODEL_TYPE[] = L"MODEL_TYPE";
static const IFXCHAR IDTF_MODEL_POSITION_COUNT[] = L"MODEL_POSITION_COUNT";
static const IFXCHAR IDTF_MODEL_BASE_POSITION_COUNT[] = L"MODEL_BASE_POSITION_COUNT";
static const IFXCHAR IDTF_MODEL_NORMAL_COUNT[] = L"MODEL_NORMAL_COUNT";
static const IFXCHAR IDTF_MODEL_DIFFUSE_COLOR_COUNT[] = L"MODEL_DIFFUSE_COLOR_COUNT";
static const IFXCHAR IDTF_MODEL_SPECULAR_COLOR_COUNT[] = L"MODEL_SPECULAR_COLOR_COUNT";
static const IFXCHAR IDTF_MODEL_TEXTURE_COORD_COUNT[] = L"MODEL_TEXTURE_COORD_COUNT";
static const IFXCHAR IDTF_MODEL_BONE_COUNT[] = L"MODEL_BONE_COUNT";
static const IFXCHAR IDTF_MODEL_SHADING_COUNT[] = L"MODEL_SHADING_COUNT";
static const IFXCHAR IDTF_MODEL_POSITION_LIST[] = L"MODEL_POSITION_LIST";
static const IFXCHAR IDTF_MODEL_BASE_POSITION_LIST[] = L"MODEL_BASE_POSITION_LIST";
static const IFXCHAR IDTF_MODEL_NORMAL_LIST[] = L"MODEL_NORMAL_LIST";
static const IFXCHAR IDTF_MODEL_TEXTURE_COORD_LIST[] = L"MODEL_TEXTURE_COORD_LIST";
static const IFXCHAR IDTF_MODEL_SHADING_DESCRIPTION_LIST[] = L"MODEL_SHADING_DESCRIPTION_LIST";
static const IFXCHAR IDTF_MODEL_DIFFUSE_COLOR_LIST[] = L"MODEL_DIFFUSE_COLOR_LIST";
static const IFXCHAR IDTF_MODEL_SPECULAR_COLOR_LIST[] = L"MODEL_SPECULAR_COLOR_LIST";
static const IFXCHAR IDTF_MODEL_SKELETON[] = L"MODEL_SKELETON";
static const IFXCHAR IDTF_BONE[] = L"BONE";
static const IFXCHAR IDTF_BONE_NAME[] = L"BONE_NAME";
static const IFXCHAR IDTF_PARENT_BONE_NAME[] = L"PARENT_BONE_NAME";
static const IFXCHAR IDTF_BONE_LENGTH[] = L"BONE_LENGTH";
static const IFXCHAR IDTF_BONE_DISPLACEMENT[] = L"BONE_DISPLACEMENT";
static const IFXCHAR IDTF_BONE_ORIENTATION[] = L"BONE_ORIENTATION";
static const IFXCHAR IDTF_BONE_LINK_COUNT[] = L"BONE_LINK_COUN";
static const IFXCHAR IDTF_BONE_LINK_LENGTH[] = L"BONE_LINK_LENGTH";
static const IFXCHAR IDTF_BONE_START_JOINT[] = L"BONE_START_JOINT";
static const IFXCHAR IDTF_BONE_END_JOINT[] = L"BONE_END_JOINT";
static const IFXCHAR IDTF_BONE_ROTATION_CONSTRAINTS_MAX[] = L"BONE_ROTATION_CONSTRAINTS_MAX";
static const IFXCHAR IDTF_BONE_ROTATION_CONSTRAINTS_MIN[] = L"BONE_ROTATION_CONSTRAINTS_MIN";

// point set resource
static const IFXCHAR IDTF_POINT_SET[] = L"POINT_SET";
static const IFXCHAR IDTF_POINT_COUNT[] = L"POINT_COUNT";
static const IFXCHAR IDTF_POINT[] = L"POINT";
static const IFXCHAR IDTF_POINT_TEX_COORD[] = L"TEX_COORD:";
static const IFXCHAR IDTF_POINT_POSITION_LIST[] = L"POINT_POSITION_LIST";
static const IFXCHAR IDTF_POINT_NORMAL_LIST[] = L"POINT_NORMAL_LIST";
static const IFXCHAR IDTF_POINT_TEXTURE_COORD_LIST[] = L"POINT_TEXTURE_COORD_LIST";
static const IFXCHAR IDTF_POINT_SHADING_LIST[] = L"POINT_SHADING_LIST";
static const IFXCHAR IDTF_POINT_DIFFUSE_COLOR_LIST[] = L"POINT_DIFFUSE_COLOR_LIST";
static const IFXCHAR IDTF_POINT_SPECULAR_COLOR_LIST[] = L"POINT_SPECULAR_COLOR_LIST";

// line set resource
static const IFXCHAR IDTF_LINE_SET[] = L"LINE_SET";
static const IFXCHAR IDTF_LINE_COUNT[] = L"LINE_COUNT";
static const IFXCHAR IDTF_LINE[] = L"LINE";
static const IFXCHAR IDTF_LINE_TEX_COORD[] = L"TEX_COORD:";
static const IFXCHAR IDTF_LINE_POSITION_LIST[] = L"LINE_POSITION_LIST";
static const IFXCHAR IDTF_LINE_NORMAL_LIST[] = L"LINE_NORMAL_LIST";
static const IFXCHAR IDTF_LINE_TEXTURE_COORD_LIST[] = L"LINE_TEXTURE_COORD_LIST";
static const IFXCHAR IDTF_LINE_SHADING_LIST[] = L"LINE_SHADING_LIST";
static const IFXCHAR IDTF_LINE_DIFFUSE_COLOR_LIST[] = L"LINE_DIFFUSE_COLOR_LIST";
static const IFXCHAR IDTF_LINE_SPECULAR_COLOR_LIST[] = L"LINE_SPECULAR_COLOR_LIST";

// mesh resource
static const IFXCHAR IDTF_MESH[] = L"MESH";
static const IFXCHAR IDTF_MESH_FACE[] = L"FACE";
static const IFXCHAR IDTF_MESH_FACE_TEX_COORD[] = L"TEX_COORD:";
static const IFXCHAR IDTF_FACE_COUNT[] = L"FACE_COUNT";
static const IFXCHAR IDTF_MESH_FACE_POSITION_LIST[] = L"MESH_FACE_POSITION_LIST";
static const IFXCHAR IDTF_MESH_FACE_NORMAL_LIST[] = L"MESH_FACE_NORMAL_LIST";
static const IFXCHAR IDTF_MESH_FACE_TEXTURE_COORD_LIST[] = L"MESH_FACE_TEXTURE_COORD_LIST";
static const IFXCHAR IDTF_MESH_FACE_SHADING_LIST[] = L"MESH_FACE_SHADING_LIST";
static const IFXCHAR IDTF_MESH_FACE_DIFFUSE_COLOR_LIST[] = L"MESH_FACE_DIFFUSE_COLOR_LIST";
static const IFXCHAR IDTF_MESH_FACE_SPECULAR_COLOR_LIST[] = L"MESH_FACE_SPECULAR_COLOR_LIST";

// shading description
static const IFXCHAR IDTF_SHADING_DESCRIPTION[] = L"SHADING_DESCRIPTION";
static const IFXCHAR IDTF_TEXTURE_LAYER_COUNT[] = L"TEXTURE_LAYER_COUNT";
static const IFXCHAR IDTF_TEXTURE_COORD_DIMENSION_LIST[] = L"TEXTURE_COORD_DIMENSION_LIST";
static const IFXCHAR IDTF_TEXTURE_LAYER[] = L"TEXTURE_LAYER";
static const IFXCHAR IDTF_TEXTURE_LAYER_DIMENSION[] = L"DIMENSION:";
static const IFXCHAR IDTF_SHADER_ID[] = L"SHADER_ID";

// shader resource
static const IFXCHAR IDTF_SHADER[] = L"SHADER";
static const IFXCHAR IDTF_SHADER_MATERIAL_NAME[] = L"SHADER_MATERIAL_NAME";
static const IFXCHAR IDTF_SHADER_ACTIVE_TEXTURE_COUNT[] = L"SHADER_ACTIVE_TEXTURE_COUNT";
static const IFXCHAR IDTF_SHADER_TEXTURE_LAYER_LIST[] = L"SHADER_TEXTURE_LAYER_LIST";
static const IFXCHAR IDTF_TEXTURE_LAYER_MODE[] = L"TEXTURE_LAYER_MODE";
static const IFXCHAR IDTF_TEXTURE_LAYER_INTENSITY[] = L"TEXTURE_LAYER_INTENSITY";
static const IFXCHAR IDTF_TEXTURE_LAYER_BLEND_FUNCTION[] = L"TEXTURE_LAYER_BLEND_FUNCTION";
static const IFXCHAR IDTF_TEXTURE_LAYER_BLEND_SOURCE[] = L"TEXTURE_LAYER_BLEND_SOURCE";
static const IFXCHAR IDTF_TEXTURE_LAYER_BLEND_CONSTANT[] = L"TEXTURE_LAYER_BLEND_CONSTANT";
static const IFXCHAR IDTF_TEXTURE_LAYER_ALPHA_ENABLED[] = L"TEXTURE_LAYER_ALPHA_ENABLED";
static const IFXCHAR IDTF_TEXTURE_NAME[] = L"TEXTURE_NAME";
static const IFXCHAR IDTF_ATTRIBUTE_LIGHTING_ENABLED[] = L"ATTRIBUTE_LIGHTING_ENABLED";
static const IFXCHAR IDTF_ATTRIBUTE_ALPHA_TEST_ENABLED[] = L"ATTRIBUTE_ALPHA_TEST_ENABLED";
static const IFXCHAR IDTF_ATTRIBUTE_USE_VERTEX_COLOR[] = L"ATTRIBUTE_USE_VERTEX_COLOR";
static const IFXCHAR IDTF_SHADER_ALPHA_TEST_REFERENCE[] = L"SHADER_ALPHA_TEST_REFERENCE";
static const IFXCHAR IDTF_SHADER_ALPHA_TEST_FUNCTION[] = L"SHADER_ALPHA_TEST_FUNCTION";
static const IFXCHAR IDTF_SHADER_COLOR_BLEND_FUNCTION[] = L"SHADER_COLOR_BLEND_FUNCTION";

// material resource
static const IFXCHAR IDTF_MATERIAL[] = L"MATERIAL";
static const IFXCHAR IDTF_ATTRIBUTE_AMBIENT_ENABLED[] = L"ATTRIBUTE_AMBIENT_ENABLED";
static const IFXCHAR IDTF_ATTRIBUTE_DIFFUSE_ENABLED[] = L"ATTRIBUTE_DIFFUSE_ENABLED";
static const IFXCHAR IDTF_ATTRIBUTE_SPECULAR_ENABLED[] = L"ATTRIBUTE_SPECULAR_ENABLED";
static const IFXCHAR IDTF_ATTRIBUTE_EMISSIVE_ENABLED[] = L"ATTRIBUTE_EMISSIVE_ENABLED";
static const IFXCHAR IDTF_ATTRIBUTE_REFLECTIVITY_ENABLED[] = L"ATTRIBUTE_REFLECTIVITY_ENABLED";
static const IFXCHAR IDTF_ATTRIBUTE_OPACITY_ENABLED[] = L"ATTRIBUTE_OPACITY_ENABLED";
static const IFXCHAR IDTF_MATERIAL_AMBIENT[] = L"MATERIAL_AMBIENT";
static const IFXCHAR IDTF_MATERIAL_DIFFUSE[] = L"MATERIAL_DIFFUSE";
static const IFXCHAR IDTF_MATERIAL_SPECULAR[] = L"MATERIAL_SPECULAR";
static const IFXCHAR IDTF_MATERIAL_EMISSIVE[] = L"MATERIAL_EMISSIVE";
static const IFXCHAR IDTF_MATERIAL_REFLECTIVITY[] = L"MATERIAL_REFLECTIVITY";
static const IFXCHAR IDTF_MATERIAL_OPACITY[] = L"MATERIAL_OPACITY";

// texture resource
static const IFXCHAR IDTF_TEXTURE[] = L"TEXTURE";
static const IFXCHAR IDTF_TEXTURE_HEIGHT[] = L"TEXTURE_HEIGHT";
static const IFXCHAR IDTF_TEXTURE_WIDTH[] = L"TEXTURE_WIDTH";
static const IFXCHAR IDTF_TEXTURE_IMAGE_TYPE[] = L"TEXTURE_IMAGE_TYPE";
static const IFXCHAR IDTF_TEXTURE_IMAGE_COUNT[] = L"TEXTURE_IMAGE_COUNT";
static const IFXCHAR IDTF_IMAGE_FORMAT_LIST[] = L"IMAGE_FORMAT_LIST";
static const IFXCHAR IDTF_IMAGE_FORMAT[] = L"IMAGE_FORMAT";
static const IFXCHAR IDTF_COMPRESSION_TYPE[] = L"COMPRESSION_TYPE";
static const IFXCHAR IDTF_ALPHA_CHANNEL[] = L"ALPHA_CHANNEL";
static const IFXCHAR IDTF_BLUE_CHANNEL[] = L"BLUE_CHANNEL";
static const IFXCHAR IDTF_GREEN_CHANNEL[] = L"GREEN_CHANNEL";
static const IFXCHAR IDTF_RED_CHANNEL[] = L"RED_CHANNEL";
static const IFXCHAR IDTF_LUMINANCE[] = L"LUMINANCE";
static const IFXCHAR IDTF_TEXTURE_PATH[] = L"TEXTURE_PATH";

static const IFXCHAR IDTF_IMAGE_TYPE_RGB[] = L"RGB";
static const IFXCHAR IDTF_IMAGE_TYPE_RGBA[] = L"RGBA";
static const IFXCHAR IDTF_IMAGE_TYPE_ALPHA[] = L"ALPHA";
static const IFXCHAR IDTF_IMAGE_TYPE_LUMINANCE[] = L"LUMINANCE";
static const IFXCHAR IDTF_IMAGE_TYPE_LUMINANCE_AND_ALPHA[] = L"LUMINANCE_AND_ALPHA";
static const IFXCHAR IDTF_IMAGE_COMPRESSION_TYPE_JPEG24[] = L"JPEG24";
static const IFXCHAR IDTF_IMAGE_COMPRESSION_TYPE_JPEG8[] = L"JPEG8";
static const IFXCHAR IDTF_IMAGE_COMPRESSION_TYPE_PNG[] = L"PNG";
static const IFXCHAR IDTF_IMAGE_COMPRESSION_TYPE_TIFF[] = L"TIFF";

// motion resource
static const IFXCHAR IDTF_MOTION[] = L"MOTION";
static const IFXCHAR IDTF_MOTION_TRACK_COUNT[] = L"MOTION_TRACK_COUNT";
static const IFXCHAR IDTF_MOTION_TRACK_LIST[] = L"MOTION_TRACK_LIST";
static const IFXCHAR IDTF_MOTION_TRACK[] = L"MOTION_TRACK";
static const IFXCHAR IDTF_MOTION_TRACK_NAME[] = L"MOTION_TRACK_NAME";
static const IFXCHAR IDTF_MOTION_TRACK_SAMPLE_COUNT[] = L"MOTION_TRACK_SAMPLE_COUNT";
static const IFXCHAR IDTF_KEY_FRAME_LIST[] = L"KEY_FRAME_LIST";
static const IFXCHAR IDTF_KEY_FRAME[] = L"KEY_FRAME";
static const IFXCHAR IDTF_KEY_FRAME_TIME[] = L"KEY_FRAME_TIME";
static const IFXCHAR IDTF_KEY_FRAME_DISPLACEMENT[] = L"KEY_FRAME_DISPLACEMENT";
static const IFXCHAR IDTF_KEY_FRAME_ROTATION[] = L"KEY_FRAME_ROTATION";
static const IFXCHAR IDTF_KEY_FRAME_SCALE[] = L"KEY_FRAME_SCALE";

// meta-data
static const IFXCHAR IDTF_META_DATA[] = L"META_DATA";
static const IFXCHAR IDTF_META_DATA_COUNT[] = L"META_DATA_COUNT";
static const IFXCHAR IDTF_META_DATA_ATTRIBUTE[] = L"META_DATA_ATTRIBUTE";
static const IFXCHAR IDTF_META_DATA_KEY[] = L"META_DATA_KEY";
static const IFXCHAR IDTF_META_DATA_VALUE[] = L"META_DATA_VALUE";
static const IFXCHAR IDTF_STRING_META_DATA[] = L"STRING";
static const IFXCHAR IDTF_BINARY_META_DATA[] = L"BINARY";

// modifiers
static const IFXCHAR IDTF_MODIFIER[] = L"MODIFIER";
static const IFXCHAR IDTF_MODIFIER_NAME[] = L"MODIFIER_NAME";
static const IFXCHAR IDTF_MODIFIER_CHAIN_TYPE[] = L"MODIFIER_CHAIN_TYPE";
static const IFXCHAR IDTF_MODIFIER_CHAIN_INDEX[] = L"MODIFIER_CHAIN_INDEX";
static const IFXCHAR IDTF_PARAMETERS[] = L"PARAMETERS";
static const IFXCHAR IDTF_ATTRIBUTES[] = L"ATTRIBUTES";

// shading modifier
static const IFXCHAR IDTF_SHADING_MODIFIER[] = L"SHADING";
static const IFXCHAR IDTF_SHADER_LIST_COUNT[] = L"SHADER_LIST_COUNT";
static const IFXCHAR IDTF_SHADER_LIST_LIST[] = L"SHADER_LIST_LIST";
static const IFXCHAR IDTF_SHADING_GROUP[] = L"SHADING_GROUP";
static const IFXCHAR IDTF_SHADER_LIST[] = L"SHADER_LIST";
static const IFXCHAR IDTF_SHADER_COUNT[] = L"SHADER_COUNT";
static const IFXCHAR IDTF_SHADER_NAME_LIST[] = L"SHADER_NAME_LIST";
static const IFXCHAR IDTF_SHADER_NAME[] = L"NAME:";
static const IFXCHAR IDTF_ATTRIBUTE_GLYPH[] = L"ATTRIBUTE_GLYPH";
static const IFXCHAR IDTF_ATTRIBUTE_MESH[] = L"ATTRIBUTE_MESH";
static const IFXCHAR IDTF_ATTRIBUTE_LINE[] = L"ATTRIBUTE_LINE";
static const IFXCHAR IDTF_ATTRIBUTE_POINT[] = L"ATTRIBUTE_POINT";

// animation modifier
static const IFXCHAR IDTF_ANIMATION_MODIFIER[] = L"ANIMATION";
static const IFXCHAR IDTF_ATTRIBUTE_ANIMATION_PLAYING[] = L"ATTRIBUTE_ANIMATION_PLAYING";
static const IFXCHAR IDTF_ATTRIBUTE_ROOT_BONE_LOCKED[] = L"ATTRIBUTE_ROOT_BONE_LOCKED";
static const IFXCHAR IDTF_ATTRIBUTE_SINGLE_TRACK[] = L"ATTRIBUTE_SINGLE_TRACK";
static const IFXCHAR IDTF_ATTRIBUTE_AUTO_BLEND[] = L"ATTRIBUTE_AUTO_BLEND";
static const IFXCHAR IDTF_TIME_SCALE[] = L"TIME_SCALE";
static const IFXCHAR IDTF_MOTION_COUNT[] = L"MOTION_COUNT";
static const IFXCHAR IDTF_MOTION_INFO_LIST[] = L"MOTION_INFO_LIST";
static const IFXCHAR IDTF_MOTION_INFO[] = L"MOTION_INFO";
static const IFXCHAR IDTF_MOTION_NAME[] = L"MOTION_NAME";
static const IFXCHAR IDTF_ATTRIBUTE_LOOP[] = L"ATTRIBUTE_LOOP";
static const IFXCHAR IDTF_ATTRIBUTE_SYNC[] = L"ATTRIBUTE_SYNC";
static const IFXCHAR IDTF_TIME_OFFSET[] = L"TIME_OFFSET";
static const IFXCHAR IDTF_BLEND_TIME[] = L"BLEND_TIME";

// bone weight modifier
static const IFXCHAR IDTF_BONE_WEIGHT_MODIFIER[] = L"BONE_WEIGHT";
static const IFXCHAR IDTF_INVERSE_QUANT[] = L"INVERSE_QUANT";
static const IFXCHAR IDTF_POSITION_COUNT[] = L"POSITION_COUNT"; 
static const IFXCHAR IDTF_POSITION_BONE_WEIGHT_LIST[] = L"POSITION_BONE_WEIGHT_LIST";
static const IFXCHAR IDTF_BONE_WEIGHT_COUNT[] = L"BONE_WEIGHT_COUNT";
static const IFXCHAR IDTF_BONE_INDEX_LIST[] = L"BONE_INDEX_LIST";
static const IFXCHAR IDTF_BONE_WEIGHT_LIST[] = L"BONE_WEIGHT_LIST";

// CLOD modifier
static const IFXCHAR IDTF_CLOD_MODIFIER[] = L"CLOD";
static const IFXCHAR IDTF_ATTRIBUTE_AUTO_LOD_CONTROL[] = L"ATTRIBUTE_AUTO_LOD_CONTROL";
static const IFXCHAR IDTF_LOD_BIAS[] = L"LOD_BIAS"; 
static const IFXCHAR IDTF_CLOD_LEVEL[] = L"CLOD_LEVEL";

// Subdivision Modifier
static const IFXCHAR IDTF_SUBDIVISION_MODIFIER[] = L"SUBDIV";
static const IFXCHAR IDTF_ATTRIBUTE_ENABLED[] = L"ATTRIBUTE_ENABLED";
static const IFXCHAR IDTF_ATTRIBUTE_ADAPTIVE[] = L"ATTRIBUTE_ADAPTIVE";
static const IFXCHAR IDTF_DEPTH[] = L"DEPTH";
static const IFXCHAR IDTF_TENSION[] = L"TENSION";
static const IFXCHAR IDTF_ERROR[] = L"ERROR";

// Glyph2D Modifier
static const IFXCHAR IDTF_GLYPH_MODIFIER[] = L"GLYPH";
static const IFXCHAR IDTF_ATTRIBUTE_BILLBOARD[] = L"ATTRIBUTE_BILLBOARD";
static const IFXCHAR IDTF_ATTRIBUTE_SINGLESHADER[] = L"ATTRIBUTE_SINGLESHADER";
static const IFXCHAR IDTF_GLYPH_COMMAND_COUNT[] = L"GLYPH_COMMAND_COUNT";
static const IFXCHAR IDTF_GLYPH_COMMAND_LIST[] = L"GLYPH_COMMAND_LIST";
static const IFXCHAR IDTF_GLYPH_COMMAND[] = L"GLYPH_COMMAND";
static const IFXCHAR IDTF_GLYPH_COMMAND_TYPE[] = L"TYPE";
static const IFXCHAR IDTF_START_GLYPH_STRING[] = L"START_GLYPH_STRING";
static const IFXCHAR IDTF_END_GLYPH_STRING[] = L"END_GLYPH_STRING";
static const IFXCHAR IDTF_START_GLYPH[] = L"START_GLYPH";
static const IFXCHAR IDTF_END_GLYPH[] = L"END_GLYPH";
static const IFXCHAR IDTF_START_PATH[] = L"START_PATH";
static const IFXCHAR IDTF_END_PATH[] = L"END_PATH";
static const IFXCHAR IDTF_MOVE_TO[] = L"MOVE_TO";
static const IFXCHAR IDTF_LINE_TO[] = L"LINE_TO";
static const IFXCHAR IDTF_CURVE_TO[] = L"CURVE_TO";
static const IFXCHAR IDTF_END_GLYPH_OFFSET_X[] = L"END_GLYPH_OFFSET_X";
static const IFXCHAR IDTF_END_GLYPH_OFFSET_Y[] = L"END_GLYPH_OFFSET_Y";
static const IFXCHAR IDTF_LINE_TO_X[] = L"LINE_TO_X";
static const IFXCHAR IDTF_LINE_TO_Y[] = L"LINE_TO_Y";
static const IFXCHAR IDTF_MOVE_TO_X[] = L"MOVE_TO_X";
static const IFXCHAR IDTF_MOVE_TO_Y[] = L"MOVE_TO_Y";
static const IFXCHAR IDTF_CONTROL1_X[] = L"CONTROL1_X";
static const IFXCHAR IDTF_CONTROL1_Y[] = L"CONTROL1_Y";
static const IFXCHAR IDTF_CONTROL2_X[] = L"CONTROL2_X";
static const IFXCHAR IDTF_CONTROL2_Y[] = L"CONTROL2_Y";
static const IFXCHAR IDTF_ENDPOINT_X[] = L"ENDPOINT_X";
static const IFXCHAR IDTF_ENDPOINT_Y[] = L"ENDPOINT_Y";
static const IFXCHAR IDTF_GLYPH_TM[] = L"GLYPH_TM";

// file reference
static const IFXCHAR IDTF_FILE_REFERENCE[] = L"FILE_REFERENCE";
static const IFXCHAR IDTF_SCOPE_NAME[] = L"SCOPE_NAME";
static const IFXCHAR IDTF_URL_COUNT[] = L"URL_COUNT";
static const IFXCHAR IDTF_URL_LIST[] = L"URL_LIST";
static const IFXCHAR IDTF_URL[] = L"URL";
static const IFXCHAR IDTF_FILTER_COUNT[] = L"FILTER_COUNT";
static const IFXCHAR IDTF_FILTER_LIST[] = L"FILTER_LIST";
static const IFXCHAR IDTF_FILTER[] = L"FILTER";
static const IFXCHAR IDTF_FILTER_TYPE[] = L"TYPE";
static const IFXCHAR IDTF_FILTER_TYPE_NAME[] = L"NAME";
static const IFXCHAR IDTF_FILTER_TYPE_TYPE[] = L"TYPE";
static const IFXCHAR IDTF_FILTER_OBJECT_NAME[] = L"OBJECT_NAME";
static const IFXCHAR IDTF_FILTER_OBJECT_TYPE[] = L"OBJECT_TYPE";
static const IFXCHAR IDTF_NAME_COLLISION_POLICY[] = L"NAME_COLLISION_POLICY";
static const IFXCHAR IDTF_WORLD_ALIAS_NAME[] = L"WORLD_ALIAS_NAME";

//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Inline functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

}

#endif
