////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


namespace sf
{
////////////////////////////////////////////////////////////
/// Default constructor
////////////////////////////////////////////////////////////
template <typename T>
ThreadLocalPtr<T>::ThreadLocalPtr(T* Value) :
ThreadLocal(Value)
{
}


////////////////////////////////////////////////////////////
/// Operator * overload to return a reference to the variable
////////////////////////////////////////////////////////////
template <typename T>
T& ThreadLocalPtr<T>::operator *() const
{
    return *static_cast<T*>(GetValue());
}


////////////////////////////////////////////////////////////
/// Operator -> overload to return a pointer to the variable
////////////////////////////////////////////////////////////
template <typename T>
T* ThreadLocalPtr<T>::operator ->() const
{
    return static_cast<T*>(GetValue());
}


////////////////////////////////////////////////////////////
/// Implicit cast operator to T*
////////////////////////////////////////////////////////////
template <typename T>
ThreadLocalPtr<T>::operator T*() const
{
    return static_cast<T*>(GetValue());
}


////////////////////////////////////////////////////////////
/// Assignment operator
////////////////////////////////////////////////////////////
template <typename T>
ThreadLocalPtr<T>& ThreadLocalPtr<T>::operator =(T* Value)
{
    SetValue(Value);
    return *this;
}


////////////////////////////////////////////////////////////
/// Assignment operator
////////////////////////////////////////////////////////////
template <typename T>
ThreadLocalPtr<T>& ThreadLocalPtr<T>::operator =(const ThreadLocalPtr<T>& Other)
{
    SetValue(Other.GetValue());
    return *this;
}

} // namespace sf
