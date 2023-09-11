//
//  logging.cpp
//  injtest
//
//  Created by Derek Li on 9/3/23.
//

#include "logging.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>

void setupConsole() {
    system("osascript -e 'tell app \"Terminal\" to do script \"clear && exec /usr/bin/tail -f classes.txt\"'");
}
