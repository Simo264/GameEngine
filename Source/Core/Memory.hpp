#pragma once

#include <memory>

template <typename T>
using UniquePointer = std::unique_ptr<T>;

template <typename T>
using SharedPointer = std::shared_ptr<T>;