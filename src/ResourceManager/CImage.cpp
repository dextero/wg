#include "CImage.h"
#include "CImageAtlas.h"
#include "../VFS/vfs.h"
#include "CResourceManager.h"

using namespace System::Resource;

CImage::CImage(): mAtlas(NULL), notified(false){}

bool CImage::dontLoadDataHack = false;

bool CImage::metaNotified = false;

void CImage::MetaNotify() const{
    if (!metaNotified){
        fprintf(stderr,"The whole image will be displayed instead");
        fprintf(stderr,"(This warning is displayed only once per image file");
        metaNotified = true;
    }
}

void CImage::LoadAtlas(CImageAtlas *atlas){
    mAtlas = atlas;
}

const sf::IntRect &CImage::GetRect(int number) {
    if (number < 0) return mRect;
    if (mAtlas == NULL){
        if (!notified){
			if (number != 0){
				fprintf(stderr,"CImage::GetRect: WARNING: requested rect %d for image %s, but no atlas was specified.\n",
					number,this->mName.c_str());
				MetaNotify();
			}
            notified = true;
        }
        return mRect;
    }
    if (number >= mAtlas->GetRectCount()){
        if (!notified){
            fprintf(stderr,"CImage::GetRect: WARNING: requested rect %d for image %s, but the atlas contains only %d rects.\n",
                number,this->mName.c_str(),mAtlas->GetRectCount());
            MetaNotify();
            notified = true;
        }
        return mRect;
    }
    return mAtlas->GetOffset(number);
}

std::string CImage::GetType()
{
	return "sf::Image";
};

bool CImage::Load(std::map<std::string,std::string>& argv)
{
	if(argv.count("name") == 0)
		return false;
	System::IO::VFSFile file = gVFS.GetFile(argv["name"].c_str());
	if(file.GetData() != NULL)
	{
		if (dontLoadDataHack){
			return true;
		}

		if(!LoadFromMemory(file.GetData(), file.GetSize()))
		{
			file.Drop();
			return false;
		}
	} else {
		return false;
	}
	file.Drop();
    mRect = sf::IntRect(0,0,GetWidth(),GetHeight());

    std::string name = argv["name"];
    if (name.find_last_of('.') != std::string::npos){
        name.erase(name.find_last_of('.'));
        name+=".atlas";

        mAtlas = gResourceManager.TryGetAtlas(name);
        if (mAtlas!=NULL)
            mAtlas->SetImage(this);
    }
	return true;
}

void CImage::Drop()
{
	delete this;
}

