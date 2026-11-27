#include <string.h>
#include "img.h"
#include "video.h"
#include "help/help.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {help(); return 1;}
    // argv[0] == ./bitfuker
    if (strcmp(argv[1], "--img") == 0) {img(argc, argv);} 
    if (strcmp(argv[1], "--vid") == 0) {video(argc, argv);}
    else {help();}
    
}