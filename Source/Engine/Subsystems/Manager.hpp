#pragma once

#include "Core/DesignPattern/Singleton.hpp"

/**
 * Every game engine requires some low-level support systems that manage
 * mundane but crucial tasks, such as starting up and shutting down the engine, 
 * configuring engine and game features, managing the engine’s memory
 * A common design patt ern for implementing major subsystems such as the ones that make up a game engine
 * is to define a singleton class (often called a manager ) for each subsystem.
 */
template<typename T>
class Manager : public Singleton<T>
{
public:
	virtual void Initialize() = 0;
	virtual void CleanUp() = 0;
};
