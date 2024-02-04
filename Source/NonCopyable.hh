#pragma once

/* Class noncopyable is a base class.
 Derive your own class from noncopyable when you want to prohibit copy construction and copy assignment. */
class NonCopyable
{
public:
	NonCopyable(const NonCopyable&) = delete;            /* Delete copy constructor */
	NonCopyable& operator=(const NonCopyable&) = delete; /* Delete assign op */

protected:
	NonCopyable() = default;
	~NonCopyable() = default;
};