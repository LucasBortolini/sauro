#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "cursos.csv"

struct Curso {
    char nomeCurso[20];
    int codigoCurso;
    float mensalidade;
    char dataVencimento[11]; // dd/mm/yyyy
    struct Curso* seguinte;
};

struct Pilha {
    struct Curso* curso;
    struct Pilha* seguinte;
};

void saveCursosToCSV(struct Curso* head, const char* filename);

struct Curso* createCurso(char* nomeCurso, int codigoCurso, float mensalidade, char* dataVencimento) {
    struct Curso* newCurso = (struct Curso*)malloc(sizeof(struct Curso));
    strcpy(newCurso->nomeCurso, nomeCurso);
    newCurso->codigoCurso = codigoCurso;
    newCurso->mensalidade = mensalidade;
    strcpy(newCurso->dataVencimento, dataVencimento);
    newCurso->seguinte = NULL;
    return newCurso;
}

void addCurso(struct Curso** head, char* nomeCurso, int codigoCurso, float mensalidade, char* dataVencimento) {
    struct Curso* newCurso = createCurso(nomeCurso, codigoCurso, mensalidade, dataVencimento);
    if (*head == NULL) {
        *head = newCurso;
    } else {
        struct Curso* temp = *head;
        while (temp->seguinte != NULL) {
            temp = temp->seguinte;
        }
        temp->seguinte = newCurso;
    }
    saveCursosToCSV(*head, ARQUIVO);
}

void saveCursosToCSV(struct Curso* head, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Não foi possível abrir o arquivo");
        return;
    }

    fprintf(file, "nomeCurso;codigoCurso;mensalidade;dataVencimento\n"); // header

    struct Curso* temp = head;
    while (temp != NULL) {
        fprintf(file, "%s;%d;%.2f;%s\n", temp->nomeCurso, temp->codigoCurso, temp->mensalidade, temp->dataVencimento);
        temp = temp->seguinte;
    }

    fclose(file);
    printf("Dados salvos em %s\n", filename);
}

void printCursos(struct Curso** head) {
    FILE* file = fopen(ARQUIVO, "r");
    if (file == NULL) {
        perror("Não foi possível abrir o arquivo");
        return;
    }

    char buffer[128];
    int line = 0;
    int existeRegistro = 0;

    // Limpa a lista atual
    while (*head != NULL) {
        struct Curso* temp = *head;
        *head = (*head)->seguinte;
        free(temp);
    }

    struct Curso* tail = NULL;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (line == 0) {
            // Ignora o header
            line++;
            continue;
        }

        existeRegistro = 1; // Encontrou pelo menos um registro

        char* nome = strtok(buffer, ";");
        int codigo = atoi(strtok(NULL, ";"));
        float mensalidade = atof(strtok(NULL, ";"));
        char* dataVencimento = strtok(NULL, "\n");

        struct Curso* newCurso = createCurso(nome, codigo, mensalidade, dataVencimento);

        if (*head == NULL) {
            *head = newCurso;
        } else {
            tail->seguinte = newCurso;
        }
        tail = newCurso;
    }

    fclose(file);

    if (!existeRegistro) {
        printf("\n Sem registros de cursos cadastrados!\n");
    } else {
        struct Curso* temp = *head;
        while (temp != NULL) {
            printf(" Nome: %s\n", temp->nomeCurso);
            printf(" Código: %d\n", temp->codigoCurso);
            printf(" Mensalidade: %.2f\n", temp->mensalidade);
            printf(" Data de Vencimento: %s\n\n", temp->dataVencimento);
            temp = temp->seguinte;
        }
    }
}

void freeCursos(struct Curso* head) {
    struct Curso* temp;
    while (head != NULL) {
        temp = head;
        head = head->seguinte;
        free(temp);
    }
}

void push(struct Pilha** top, struct Curso* curso) {
    struct Pilha* newElement = (struct Pilha*)malloc(sizeof(struct Pilha));
    newElement->curso = curso;
    newElement->seguinte = *top;
    *top = newElement;
}

struct Curso* pop(struct Pilha** top) {
    if (*top == NULL) {
        return NULL;
    }
    struct Pilha* temp = *top;
    *top = (*top)->seguinte;
    struct Curso* curso = temp->curso;
    free(temp);
    return curso;
}

void cadastrarCurso(struct Curso** cursoHead) {
    char nomeCurso[20];
    int codigoCurso;
    float mensalidade;
    char dataVencimento[11];

    printf("\n Digite o nome do curso: ");
    scanf("%19s", nomeCurso);
    printf(" Digite o código curso: ");
    scanf("%d", &codigoCurso);
    printf(" Digite a mensalidade do curso: ");
    scanf("%f", &mensalidade);
    printf(" Digite a data de vencimento (dd/mm/yyyy): ");
    scanf("%10s", dataVencimento);

    addCurso(cursoHead, nomeCurso, codigoCurso, mensalidade, dataVencimento);
    printf(" Curso adicionado com sucesso!\n");
}

void eliminarCurso(struct Curso** head, struct Pilha** undoStack, int codigo) {
    struct Curso* temp = *head;
    struct Curso* prev = NULL;

    // Caso o head da lista seja o curso a ser removido
    if (temp != NULL && temp->codigoCurso == codigo) {
        *head = temp->seguinte; // Muda o head
        push(undoStack, temp); // Empilha o curso removido para possível desfazer
        saveCursosToCSV(*head, ARQUIVO);
        printf("Curso com código %d removido com sucesso!\n", codigo);
        return;
    }

    // Procura pelo curso a ser removido, mantendo o ponteiro para o nó anterior
    while (temp != NULL && temp->codigoCurso != codigo) {
        prev = temp;
        temp = temp->seguinte;
    }

    // Se o código não foi encontrado
    if (temp == NULL) {
        printf("Curso com código %d não encontrado!\n", codigo);
        return;
    }

    // Desvincula o nó da lista
    prev->seguinte = temp->seguinte;
    push(undoStack, temp); // Empilha o curso removido para possível desfazer
    saveCursosToCSV(*head, ARQUIVO);
    printf("Curso com código %d removido com sucesso!\n", codigo);
}

void alterarCodigoCurso(struct Curso* head, int codigoAntigo, int codigoNovo) {
    struct Curso* temp = head;

    while (temp != NULL) {
        if (temp->codigoCurso == codigoAntigo) {
            temp->codigoCurso = codigoNovo;
            saveCursosToCSV(head, ARQUIVO);
            printf("Código do curso alterado com sucesso!\n");
            return;
        }
        temp = temp->seguinte;
    }
    printf("Curso com código %d não encontrado!\n", codigoAntigo);
}

void printCursoPorCodigo(struct Curso* head, int codigo) {
    while (head != NULL) {
        if (head->codigoCurso == codigo) {
            printf("\nDados do curso com código %d:\n", codigo);
            printf(" Nome do Curso: %s\n", head->nomeCurso);
            printf(" Código: %d\n", head->codigoCurso);
            printf(" Mensalidade: %.2f\n", head->mensalidade);
            printf(" Data de Vencimento: %s\n\n", head->dataVencimento);
            return;
        }
        head = head->seguinte;
    }
    printf("Curso com matrícula %d não encontrado!\n", codigo);
}

void bubbleSortCursos(struct Curso** head) {
    if (*head == NULL) {
        return;
    }

    int swapped;
    struct Curso *ptr1;
    struct Curso *lptr = NULL;

    do {
        swapped = 0;
        ptr1 = *head;

        while (ptr1->seguinte != lptr) {
            if (ptr1->codigoCurso > ptr1->seguinte->codigoCurso) {
                // Troca os dados dos nós
                char tempNomeCurso[20];
                strcpy(tempNomeCurso, ptr1->nomeCurso);
                strcpy(ptr1->nomeCurso, ptr1->seguinte->nomeCurso);
                strcpy(ptr1->seguinte->nomeCurso, tempNomeCurso);

                int tempCodigoCurso = ptr1->codigoCurso;
                ptr1->codigoCurso = ptr1->seguinte->codigoCurso;
                ptr1->seguinte->codigoCurso = tempCodigoCurso;

                float tempMensalidade = ptr1->mensalidade;
                ptr1->mensalidade = ptr1->seguinte->mensalidade;
                ptr1->seguinte->mensalidade = tempMensalidade;

                char tempDataVencimento[11];
                strcpy(tempDataVencimento, ptr1->dataVencimento);
                strcpy(ptr1->dataVencimento, ptr1->seguinte->dataVencimento);
                strcpy(ptr1->seguinte->dataVencimento, tempDataVencimento);

                swapped = 1;
            }
            ptr1 = ptr1->seguinte;
        }
        lptr = ptr1;
    } while (swapped);

    printf("Cursos ordenados com sucesso!\n");
}

void printCursoPorNome(struct Curso* head, char* nome) {
    while (head != NULL) {
        if (strcmp(head->nomeCurso, nome) == 0) {
            printf("\nDados do curso com nome %s:\n", nome);
            printf(" Nome do Curso: %s\n", head->nomeCurso);
            printf(" Código: %d\n", head->codigoCurso);
            printf(" Mensalidade: %.2f\n", head->mensalidade);
            printf(" Data de Vencimento: %s\n\n", head->dataVencimento);
            return;
        }
        head = head->seguinte;
    }
    printf("Curso com nome %s não encontrado!\n", nome);
}

int main() {
    struct Curso* cursoHead = NULL;
    struct Pilha* undoStack = NULL;
    int escolha, codigoAntigo, codigoNovo;
    char nomeCurso[20];

    do {
        printf("\n----- Menu -----\n");
        printf("1. Cadastrar curso\n");
        printf("2. Imprimir cursos\n");
        printf("3. Remover curso\n");
        printf("4. Alterar código de curso\n");
        printf("5. Buscar curso por código\n");
        printf("6. Ordenar cursos\n");
        printf("7. Buscar curso por nome\n");
        printf("8. Desfazer última remoção de curso\n");
        printf("9. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &escolha);

        switch (escolha) {
            case 1:
                cadastrarCurso(&cursoHead);
                break;
            case 2:
                printCursos(&cursoHead);
                break;
            case 3:
                printf("Digite o código do curso a ser removido: ");
                scanf("%d", &codigoAntigo);
                eliminarCurso(&cursoHead, &undoStack, codigoAntigo);
                break;
            case 4:
                printf("Digite o código antigo do curso: ");
                scanf("%d", &codigoAntigo);
                printf("Digite o novo código do curso: ");
                scanf("%d", &codigoNovo);
                alterarCodigoCurso(cursoHead, codigoAntigo, codigoNovo);
                break;
            case 5:
                printf("Digite o código do curso a ser buscado: ");
                scanf("%d", &codigoAntigo);
                printCursoPorCodigo(cursoHead, codigoAntigo);
                break;
            case 6:
                bubbleSortCursos(&cursoHead);
                break;
            case 7:
                printf("Digite o nome do curso a ser buscado: ");
                scanf("%19s", nomeCurso);
                printCursoPorNome(cursoHead, nomeCurso);
                break;
            case 8:
                {
                    struct Curso* cursoDesfeito = pop(&undoStack);
                    if (cursoDesfeito != NULL) {
                        cursoDesfeito->seguinte = cursoHead;
                        cursoHead = cursoDesfeito;
                        saveCursosToCSV(cursoHead, ARQUIVO);
                        printf("Desfazer realizado com sucesso!\n");
                    } else {
                        printf("Nenhuma ação de remoção para desfazer!\n");
                    }
                }
                break;
            case 9:
                printf("Saindo do programa...\n");
                break;
            default:
                printf("Opção inválida. Por favor, escolha uma opção válida.\n");
                break;
        }
    } while (escolha != 9);

    freeCursos(cursoHead);
    return 0;
}
