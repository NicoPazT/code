/* Routine for evaluating population members  */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include <time.h>

# include "global.h"
# include "rand.h"

/* Función para convertir xbin a lista binaria (0 o 1) */
void convertir_a_lista_binaria(const double *xbin, int *lista) {
    int j;
    for (j = 0; j < nbin; j++) {
        if (xbin[j] > 0.5) {
            lista[j] = 1;
        } else {
            lista[j] = 0;
        }
    }
}

void crear_lista_binaria_reducida(individual *ind, int num_variables) {
    int i;
    
    for (i = 0; i < nbin; i++) {
        ind->xbin_red[i] = 0.0;
    }

    for (i = 0; i < num_variables; i++) {
        int indice = ind->characts[i] - 1;
        
        if (indice >= 0 && indice < nbin) {
            ind->xbin_red[indice] = 1.0;
        }
    }
}

void construir_comando_awk(char *comando, const int *lista) {
    comando[0] = '\0';
    int i;
    strcpy(comando, "awk -F, '{OFS=\",\";print $1,");
    
    int primero = 1;
    char buffer[100]; 
    
    /*printf("DEBUG: nbin = %d\n", nbin);*/
    
    int max_columns = nbin - 1;
    if (max_columns <= 0) {
        fprintf(stderr, "Error: Invalid number of columns\n");
        return;
    }

    /*for (i = 0; i < max_columns; i++) {
        printf("Lista[%d] = %d\n", i, lista[i]);
    }*/

    for (i = 0; i < max_columns; i++) {
        if (lista[i] == 1) {
            if (strlen(comando) + 10 >= 50000) { 
                fprintf(stderr, "Buffer overflow prevented\n");
                break;
            }
            
            if (!primero) {
                strcat(comando, ",");
            }
            snprintf(buffer, sizeof(buffer), "$%d", i + 2); 
            strcat(comando, buffer);
            primero = 0;
        }
    }

    strcat(comando, "}' ");
    strcat(comando, csv);
    strcat(comando, " > cols_random.csv");

    /*printf("Constructed AWK command: %s\n", comando);*/
}

double ejecutar_script_R(const char *csv_file, int seed, int *num_variables, individual *ind) {
    char comando[256];
    double acc = 0.0;
    int i;

    if (ind->characts != NULL) {
        free(ind->characts);
        ind->characts = NULL;
    }

    snprintf(comando, sizeof(comando), "Rscript script3.R %s %d > scriptR.csv\n", csv_file, seed);

    clock_t inicio = clock();
    system(comando);
    clock_t fin = clock();
    double tiempo_transcurrido = ((double)(fin - inicio)) / CLOCKS_PER_SEC;

    FILE *file = fopen("scriptR.csv", "r");
    fscanf(file, "%lf %d", &acc, num_variables);

    ind->characts = (int *)malloc(*num_variables * sizeof(int));

    for (i = 0; i < *num_variables; i++) {
        fscanf(file, "%d", &(ind->characts[i]));
    }
    /*printf("Número de variables: %d\n", *num_variables);*/

    fclose(file);

    printf("Características utilizadas:\n");
    for (i = 0; i < *num_variables; i++) {
        printf("%d ", ind->characts[i]);
    }
    printf("\n");

    printf("Tiempo de ejecución de Rscript: %f segundos\n", tiempo_transcurrido);

    return acc;
}

void evaluate_pop (population *pop)
{
    int i;
    for (i=0; i<popsize; i++)
    {
        evaluate_ind (&(pop->ind[i]));
    }
    return;
}

void evaluate_ind (individual *ind)
{
    int *lista_binaria = (int *)malloc(nbin * sizeof(int));
    convertir_a_lista_binaria(ind->xbin, lista_binaria);
    int j;

    int num_unos = 0;
    for (j = 0; j < nbin; j++) {
        if (lista_binaria[j] == 1) {
            num_unos++;
        }
    }
    
    char comando[50000];
    construir_comando_awk(comando, lista_binaria);

    clock_t inicio = clock();
    system("rm -f cols_random.csv scripR.csv");
    system(comando);
    clock_t fin = clock();
    double tiempo_transcurrido = ((double)(fin - inicio)) / CLOCKS_PER_SEC;

    int num_variables;
    double acc = ejecutar_script_R("cols_random", 100, &num_variables, ind);

    crear_lista_binaria_reducida(ind, num_variables);
    
    if (acc != -1) {
        ind->obj[0] = acc * -1;       
        ind->obj[1] = num_variables;
        /*printf("Obj 2: %d\n", ind->obj[1]);*/
    } else {
        ind->obj[0] = 0.0;
        ind->obj[1] = 0;
    }

    printf("Tiempo de ejecución de awk: %f segundos\n", tiempo_transcurrido);
    printf("Número de columnas seleccionadas: %d\n", num_unos);

    free(lista_binaria);

    if (ncon==0)
    {
        ind->constr_violation = 0.0;
    }
    else
    {
        ind->constr_violation = 0.0;
        for (j=0; j<ncon; j++)
        {
            if (ind->constr[j]<0.0)
            {
                ind->constr_violation += ind->constr[j];
            }
        }
    }
    return;
}