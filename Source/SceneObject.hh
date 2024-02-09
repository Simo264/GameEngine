#pragma once

#include "Core.hh"
#include "NonCopyable.hh"

/* -------------------------------------------------------------
	SceneObject class represents a specific object placed in world 
	------------------------------------------------------------- */
template <class T>
class SceneObject : public NonCopyable {
public:
	SceneObject(T* obj = nullptr, bool visible = false) : object{ obj }, visible{ visible }
	{
		objectID = _id++;
		SetName();
	}

	bool Compare(const SceneObject& other)
	{
		return this->objectID == other.objectID;
	}

	/* Allocate on heap memory new scene object of type T */
	static SceneObject<T>* Create(T* t, bool visible = true)
	{
		return new SceneObject<T>(t, visible);
	}

	/* Free memory */
	static void DestroyObject(SceneObject<T>* o)
	{
		delete o;
	}


	T* object;
	String name;
	bool visible;
	uint32_t objectID;

private:
	inline static uint32_t _id = 0;

	void SetName();
};
