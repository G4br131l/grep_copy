#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__linux__)
    #include <unistd.h>
#else
    #include <direct.h>
#endif


#define RED   "\x1B[31m"
#define RESET "\x1B[0m"

#define MAXnomeDeArquivo 50
#define MAXlistaDeArquivos 100
#define MAIORlinhasuportada 512

typedef struct 
{
    int tamanho;
    char **lista;
} string;


void listar_arquivos(char ***listaDeArquivos, int *tamLista);
char *colorir_palavra(char linha[], char palavra[]);
int buscar_string_arquivo(char *nomeArquivo, char busca[], char *linha);
int buscar_string_diretorio(char *lista, char busca[]);


int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        printf("falta de argumento ... <string de busca\n>");
        return 0;
    }

    char stringDeBusca[MAIORlinhasuportada];

    strcpy(stringDeBusca, argv[1]);

    string listaDeArquivos;
    
    listar_arquivos(&listaDeArquivos.lista, &listaDeArquivos.tamanho);

    char local_linha[MAIORlinhasuportada];

    printf("arquivos: \n");

    for (int i = 0; i < listaDeArquivos.tamanho; i++)
    {
        int local = buscar_string_arquivo(listaDeArquivos.lista[i], stringDeBusca, local_linha);
        if (local) {
            printf("\n%s\n%s linha: %d\n", local_linha, listaDeArquivos.lista[i], local);
        }
    }

    printf("\ndiretorios:\n\n");

    for (int i = 0; i < listaDeArquivos.tamanho; i++)
    {
        if (buscar_string_diretorio(listaDeArquivos.lista[i], stringDeBusca))
        {
            printf("%s\n", listaDeArquivos.lista[i]);
        }
    }
    
}

#if defined(__linux__)
    void listar_arquivos(char ***listaDeArquivos, int *tamLista) {
        system("find . -type f > arquivos.txt");

        FILE *lista = fopen("arquivos.txt", "r");
        if (lista == NULL)
        {
            printf("erro ao abrir arquivo\n");
            return;
        }
        
        *listaDeArquivos = (char**)malloc(MAXlistaDeArquivos * sizeof(char*));

        char arquivo[MAXnomeDeArquivo];
        *tamLista = 0;

        while (fscanf(lista," %s ", arquivo) != EOF)
        {
            if (strstr(arquivo, ".txt")) {
                (*listaDeArquivos)[*tamLista] = strdup(arquivo);
                (*tamLista)++;
            }
        }

        fclose(lista);
        remove("arquivos.txt");
    }
#else
    void listar_arquivos(char ***listaDeArquivos, int *tamLista) {
        char current_path[128];

        if (_getcwd(current_path, sizeof(current_path)) == NULL) {
            printf("erro no caminho\n");
            return;
        }

        char comando[256] = "powershell -Command dir ";
        strcat(comando,current_path);
        strcat(comando, " -File -Recurse -Name > arquivos.txt");
        
        system(comando);

        FILE *lista = fopen("arquivos.txt", "r");
        if (lista == NULL)
        {
            printf("erro ao abrir arquivo\n");
            return;
        }
        
        *listaDeArquivos = (char**) malloc(MAXlistaDeArquivos * sizeof(char*));

        char arquivo[MAXnomeDeArquivo];
        *tamLista = 0;

        while (fscanf(lista," %s ", arquivo) != EOF)
        {
            if (/* strstr(arquivo, ".txt") */ 1) {
                (*listaDeArquivos)[*tamLista] = strdup(arquivo);
                (*tamLista)++;
            }
        }

        fclose(lista);
        //remove("arquivos.txt");
    }
#endif

int buscar_string_arquivo(char *nomeArquivo, char busca[], char *linha) {
    if (!strstr(nomeArquivo , ".txt")) {
        return 0;
    }
    FILE *arquivo = fopen(nomeArquivo, "r");

    int local_linha = 0;
    int flag = 0;

    while (fscanf(arquivo, " %[^\n] ", linha) != EOF)
    {
        local_linha++;
        if (strstr(linha, busca)) {
            flag = 1;
            break;
        }
    }

    fclose(arquivo);
    
    if (flag)
    {
        strcpy(linha, colorir_palavra(linha, busca));
        return local_linha;
    }
    return flag;
}

char *colorir_palavra(char linha[], char palavra[]) {
    char *ret = strstr(linha, palavra);

    char *saida = (char *) calloc(MAIORlinhasuportada, sizeof(char));
    if (saida == NULL) {
        printf("erro de alocacao");
        return NULL;
    }

    if (ret - linha != 0)
    {
        char *antes = ret - 1;
        char temp = antes[0];
        *antes = '\0';
        sprintf(antes + strlen(antes), "%c", temp); //concatena caractere
    }

    char *depois = ret + strlen(palavra);

    if (ret - linha != 0) strcat(saida, linha);
    strcat(saida , RED);
    strcat(saida, palavra);
    strcat(saida, RESET);
    strcat(saida, depois);
    
    return saida;
}

int buscar_string_diretorio(char *lista, char busca[]) {
    if(strstr(lista, busca)) {
        strcpy(lista ,colorir_palavra(lista, busca));
        return 1;
    }
    
    return 0;
}