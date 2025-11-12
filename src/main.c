#include <stdio.h>
#include <stdlib.h>
#include "dyn_array.h"

// Generate de 2 array typer
// Den første har elementer af typen `int` og navnet: `DynArrayCustom`
CREATE_DYNAMIC_ARRAY(int, Custom);
// Den anden har elementer af typen `float` og navnet: `DynArrayFloat`
CREATE_DYNAMIC_ARRAY(float, Float);

int main() {
    ///////////////////////////////////////////////////////////////////
    ///                   BLACK MAGIC ARRAYS                        ///
    ///////////////////////////////////////////////////////////////////

    // My Custom dynamic array using `int`
    DynArrayCustom arr = dynArrayCustomInit(0);

    // Tilføg elemetet `5` til array
    dynArrayCustomAppend(&arr, 5);


    // My Float dynamic array using `float`
    DynArrayFloat arr2 = dynArrayFloatInit(0);

    // Tilføg et element med værdien `0.1f + 0.25f` til array
    dynArrayFloatAppend(&arr2, 0.1f + 0.25f);

    printf("Custom array value: %d\n", arr.elements[0]);
    printf("Float array value: %.3f\n", arr2.elements[0]);

    ///////////////////////////////////////////////////////////////////
    ///                   Mere normale dynamic arrays               ///
    ///////////////////////////////////////////////////////////////////

    // Ingen suffix i dens navn ligesom `Custom` og `Float`
    // Her skal vi også manuelt give den størrelsen af vores type
    DynArray normal_arr = dynArrayInit(0, sizeof(double));

    const double item = 5.555555;

    // Siden at den her funktion ikke ved hvilken type at arrayet har
    // skal vi give den en pointer til item, vi kan ikke bare give item direkte
    dynArrayAppend(&normal_arr, &item);

    // Very ugly item retrieval
    // const double value = ((double*)normal_arr.elements)[0];
    // 
    // Har derfor lavet en hjælper funktion til dette, som giver en pointer

    const double* value = dynArrayGet(&normal_arr, 0);
    printf("\nNormal array value: %lf\n", *value);
}
