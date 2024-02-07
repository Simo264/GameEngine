#pragma once

#include "Core.hh"
#include "NonCopyable.hh"

/* SceneObject class represents a specific object placed in world */

template <typename T>
class SceneObject : public NonCopyable {
public:
	SceneObject(const char* name)
	{
		object = nullptr;
		visible = false;
		objectID = _id++;
		this->name = name;
	}
	SceneObject(T* obj, bool b, const char* name)
	{
		object = obj;
		visible = b;
		objectID = _id++;
		this->name = name;
	}

	String ToString()
	{
		OStringStream oss;
		if (!object)
			oss << "Object not valid!\n";
		else
			oss << "Name=" << name << "\nVisible: " << visible << "\nID=" << objectID << "\n";
		return oss.str();
	}

	bool Compare(const SceneObject& other)
	{
		return this->objectID == other.objectID;
	}

	/* Allocate on heap memory new scene object of type T */
	static SceneObject<T>* Create(T* t, bool visible, const char* name)
	{
		return new SceneObject<T>(t, visible, name);
	}


	T* object;
	String name;
	bool visible;
	uint32_t objectID;

private:
	inline static uint32_t _id = 0;
};

