#pragma once

namespace UniformConstants
{

inline constexpr const char* DCOLOR                = "ddColor";
inline constexpr const char* COLOR                 = "dColor";
inline constexpr const char* TEXTURE_SAMPLER       = "textureSampler";
inline constexpr const char* FONT_TEXTURE_SAMPLER  = "fontTextureSampler";
inline constexpr const char* TRANSFORMATION_MATRIX = "transformationMatrix";
inline constexpr const char* CAMERA_UNIFORMS       = "cameraUniforms";
inline constexpr const char* ORTHO_UNIFORM         = "orthoMatrix";

// World Specific Uniforms
inline constexpr const char* WORLD_GRIDSIZE  = "gridSize";
inline constexpr const char* WORLD_TILEARRAY = "tileFlags";
}  // namespace UniformConstants
