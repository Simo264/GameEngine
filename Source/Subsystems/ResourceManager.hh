#pragma once

#include "../Core.hh"
#include "../NonCopyable.hh"

/*  --------------------------------------------------------------
	This class represents the interface of a resource manager class,
	i.e TexturesManager, ShadersManager...
	This interface class implement the singleton design pattern.
	 --------------------------------------------------------------- */
template<typename T>
class ResourceManagerInterface : public NonCopyable
{
public:
	virtual void Initialize() = 0;
	virtual void ShutDown() = 0;

	static T& Instance()
	{
		/* Guaranteed to be destroyed. Instantiated on first use */
		static T instance;
		return instance;
	}

protected:
	ResourceManagerInterface() = default;
	~ResourceManagerInterface() = default;
};

