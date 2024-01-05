#pragma once

// Class defining a non-copyable object
// -------------------------------------------------
class UncopyableObject
{
public:
	UncopyableObject()	= default;
	~UncopyableObject() = default;

	UncopyableObject(const UncopyableObject&) = delete;            // delete copy constructor
	UncopyableObject& operator=(const UncopyableObject&) = delete; // delete assign op
};