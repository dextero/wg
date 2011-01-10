#ifndef __IRESOURCE_H__
#define __IRESOURCE_H__

#include <string>
#include <map>

typedef unsigned long long ResourceHandle;
#define InvalidResourceHandle 0

class IResource
{
public:
    virtual ~IResource() {};

    ResourceHandle mGid;
    std::string mName;

    virtual std::string GetType() = 0;
    virtual bool Load(std::map<std::string,std::string>& argv) = 0;
    virtual void Drop() = 0;
};
#endif

