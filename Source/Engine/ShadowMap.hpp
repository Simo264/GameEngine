#pragma once

#include "Core/Core.hpp"

/**
 * The idea behind shadow mapping is quite simple: we render the scene from the light's point of view 
 * and everything we see from the light's perspective is lit and everything we can't see must be in shadow.
 * We want to get the point on the ray where it first hit an object and compare this closest point to other 
 * points on this ray. We then do a basic test to see if a test point's ray position is further down the ray 
 * than the closest point and if so, the test point must be in shadow.
 * We use the depth buffer.
 * 
 * A value in the depth buffer corresponds to the depth of a fragment clamped to [0,1] from the 
 * camera's point of view. 
 * We render the scene from the light's perspective and store the resulting depth values 
 * in a texture. This way, we can sample the closest depth values as seen from the light's perspective. 
 * After all, the depth values show the first fragment visible from the light's perspective. 
 * We store all these depth values in a texture that we call a depth map (or shadow map).
 * 
 * Shadow mapping therefore consists of two passes: 
 * 	1. first we render the depth map
 * 	2. render the scene as normal and use the generated depth map to calculate whether fragments are in shadow.
 * 
 * The depth map:
 * the first pass requires us to generate a depth map. The depth map is the depth texture as rendered from 
 * the light's perspective that we'll be using for testing for shadows. 
 * Because we need to store the rendered result of a scene into a texture we're going to need framebuffers again.
 */
class ShadowMap
{
public:
	/**
	 * @param width: the resolution of the depth map
	 * @param height: the resolution of the depth map
	*/
	ShadowMap(int width, int height);
	
	~ShadowMap();

	/**
	 * Bind framebuffer object 
	 */
	void Bind() const;

	/**
	 * Unbind framebuffer object
	 */
	void Unbind() const;

	constexpr uint32_t GetDepthTexture() const { return _depthMap; }

private:
	uint32_t _fbo; /* framebuffer id */

	uint32_t _depthMap;	/* texture id */

	bool CheckStatus() const;
};