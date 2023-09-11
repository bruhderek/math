//
//  image.hpp
//  math
//
//  Created by Derek Li on 9/20/23.
//

#include <stdio.h>
#include <sys/types.h>
#include <mach/error.h>
#include <mach/vm_types.h>
#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#include <mach/mach.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <string>
#include <iostream>

#ifndef image_hpp
#define image_hpp

std::string getFileNameFromPath(const std::string& path)
{
    int pos = (int)path.length();
    while (pos && path[pos] != '/') --pos;
    return path.substr(pos + 1);
}

namespace Util {
void getAllImages() {
    unsigned int imageCount = _dyld_image_count();
    for (int i = 0; i < imageCount; i++) {
        mach_header *header = (mach_header *) _dyld_get_image_header(i);
        
        const char *imageName = _dyld_get_image_name(i);
        if (!imageName) continue;
        
        std::string shortName = getFileNameFromPath(std::string(imageName));
        
        std::cout << "imageName " << imageName << std::endl;
        std::cout << "shortName " << shortName << std::endl;
        
        struct stat sb;
        if (stat(imageName, &sb)) {
            break;
        }
    }
}
}

#endif /* image_hpp */
