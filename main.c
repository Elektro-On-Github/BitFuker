#include <string.h>
#include "img.h"
#include "video.h"
#include "help/help.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {help();return 0;} // campo ristretto per i param. avoidiamo roba indefinita
    // argv[0] == ./bitfuker
    if (strcmp(argv[1], "--img") == 0) {img(argv);} 
    if (strcmp(argv[1], "--vid") == 0) {video(argv);}
    else {help();}
    
}