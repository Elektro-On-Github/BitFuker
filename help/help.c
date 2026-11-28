#include <stdio.h>
#include <stdlib.h>

void help() {
    printf("Usage: bitfuker --type --file-path --fx\n");
    printf("Effects:\n");
    printf("  --horizontal-line     Genera linee verticali con byte casuali\n");
    printf("  --xor               Applica l'operazione XOR ai pixel\n");
    printf("  --saturate          Overflow controllato con i valori dei pixel\n");
    printf("  --bug               Non è un bug, è una feature!\n");
    printf("  --nintendo-3ds      Effetto simile al 3D della N3DS\n");
    printf("  --soft-ghost        Ghosting (soft)\n");
    printf("  --hard-ghost        Ghosting (hard)\n");
    printf("  --granuly           Applica un effetto granulare\n");
    printf("  --blacknoise        Rumore nero\n");
    printf("  --huge-distortion   Distorsione gigantesca");
    printf("\n");
    exit(EXIT_SUCCESS);
}