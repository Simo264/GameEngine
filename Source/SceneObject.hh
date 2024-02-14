#pragma once

#include "Core.hh"
#include "NonCopyable.hh"

/* ------------------------------------------------------------------
	SceneObject class represents a specific object placed in world,
	contains more necessary informations about an 
	object that in scene like id, tag name, visibility...
	------------------------------------------------------------------- */
class SceneObject : public NonCopyable
{
public:
	SceneObject();

	bool visible;
	String tagName;
	
	uint32_t GetID() const { return _instanceID; }
	bool Compare(const SceneObject& other) const;

private:
	uint32_t _instanceID;
	inline static uint32_t _id = 0;
};
