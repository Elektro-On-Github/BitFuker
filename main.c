#include <string.h>
#include "img.h"

int main(int argc, char *argv[]) {
    // argv[0] == ./bitfuker
    if (strcmp(argv[1], "--img") == 0) {img(int argc, char *argv[])} 
    if (strcmd(argv[1], "--vid") == 0) {video(int argc, char *argv[])}
    
}