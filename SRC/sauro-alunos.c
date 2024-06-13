#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "alunos.csv"

struct Aluno {
    char nomeAluno[20];
    char sobrenomeAluno[20];
    char dataNascimentoAluno[11]; // DD-MM-YYYY
    int matriculaAluno;
    struct Aluno* seguinte;
};

void saveAlunosToCSV(struct Aluno* head, const char* filename);

struct Aluno* createAluno(char* nomeAluno, char* sobrenomeAluno, char* dataNascimentoAluno, int matriculaAluno) {
    struct Aluno* newAluno = (struct Aluno*)malloc(sizeof(struct Aluno));
    strcpy(newAluno->nomeAluno, nomeAluno);
    strcpy(newAluno->sobrenomeAluno, sobrenomeAluno);
    strcpy(newAluno->dataNascimentoAluno, dataNascimentoAluno);
    newAluno->matriculaAluno = matriculaAluno;
    newAluno->seguinte = NULL;
    return newAluno;
}

void addAluno(struct Aluno** head, char* nomeAluno, char* sobrenomeAluno, char* dataNascimentoAluno, int matriculaAluno) {
    struct Aluno* newAluno = createAluno(nomeAluno, sobrenomeAluno, dataNascimentoAluno, matriculaAluno);
    if (*head == NULL) {
        *head = newAluno;
    } else {
        struct Aluno* temp = *head;
        while (temp->seguinte != NULL) {
            temp = temp->seguinte;
        }
        temp->seguinte = newAluno;
    }
    saveAlunosToCSV(*head, ARQUIVO);
}

void saveAlunosToCSV(struct Aluno* head, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Não foi possível abrir o arquivo");
        return;
    }

    fprintf(file, "Nome;Sobrenome;DatadeNascimento;Matrícula\n"); // header

    struct Aluno* temp = head;
    while (temp != NULL) {
        fprintf(file, "%s;%s;%s;%d\n", temp->nomeAluno, temp->sobrenomeAluno, temp->dataNascimentoAluno,
                temp->matriculaAluno);
        temp = temp->seguinte;
    }

    fclose(file);
    printf("Dados salvos em %s\n", filename);
}

void printAlunos(struct Aluno** head) {
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
        struct Aluno* temp = *head;
        *head = (*head)->seguinte;
        free(temp);
    }

    struct Aluno* tail = NULL;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (line == 0) {
            // Ignora o header
            line++;
            continue;
        }

        existeRegistro = 1; // Encontrou pelo menos um registro

        char* nome = strtok(buffer, ";");
        char* sobrenome = strtok(NULL, ";");
        char* dataNascimento = strtok(NULL, ";");
        int matricula = atoi(strtok(NULL, ";"));

        struct Aluno* newAluno = createAluno(nome, sobrenome, dataNascimento, matricula);

        if (*head == NULL) {
            *head = newAluno;
        } else {
            tail->seguinte = newAluno;
        }
        tail = newAluno;
    }

    fclose(file);

    if (!existeRegistro) {
        printf("\n Sem registros de alunos cadastrados!\n");
    } else {
        struct Aluno* temp = *head;
        while (temp != NULL) {
            printf(" Nome: %s\n", temp->nomeAluno);
            printf(" Sobrenome: %s\n", temp->sobrenomeAluno);
            printf(" Data de Nascimento: %s\n", temp->dataNascimentoAluno);
            printf(" Matrícula: %d\n\n", temp->matriculaAluno);
            temp = temp->seguinte;
        }
    }
}

void freeAlunos(struct Aluno* head) {
    struct Aluno* temp;
    while (head != NULL) {
        temp = head;
        head = head->seguinte;
        free(temp);
    }
}

void cadastrarAluno(struct Aluno** alunoHead) {
    char nomeAluno[20];
    char sobrenomeAluno[20];
    char dataNascimentoAluno[11];
    int matriculaAluno;

    printf("\n Digite o nome: ");
    scanf("%19s", nomeAluno);
    printf(" Digite o sobrenome: ");
    scanf("%19s", sobrenomeAluno);
    printf(" Digite a data de nascimento (DD-MM-YYYY): ");
    scanf("%10s", dataNascimentoAluno);
    printf(" Digite a matrícula: ");
    scanf("%d", &matriculaAluno);

    addAluno(alunoHead, nomeAluno, sobrenomeAluno, dataNascimentoAluno, matriculaAluno);
    printf(" Aluno adicionado com sucesso!\n");
}

void eliminarAluno(struct Aluno** head, int matricula) {
    struct Aluno* temp = *head;
    struct Aluno* prev = NULL;

    // Caso o head da lista seja o aluno a ser removido
    if (temp != NULL && temp->matriculaAluno == matricula) {
        *head = temp->seguinte; // Muda o head
        free(temp); // Libera o antigo head
        saveAlunosToCSV(*head, ARQUIVO);
        printf("Aluno com matrícula %d removido com sucesso!\n", matricula);
        return;
    }

    // Procura pelo aluno a ser removido, mantendo o ponteiro para o nó anterior
    while (temp != NULL && temp->matriculaAluno != matricula) {
        prev = temp;
        temp = temp->seguinte;
    }

    // Se a matrícula não foi encontrada
    if (temp == NULL) {
        printf("Aluno com matrícula %d não encontrado!\n", matricula);
        return;
    }

    // Desvincula o nó da lista
    prev->seguinte = temp->seguinte;
    free(temp);
    saveAlunosToCSV(*head, ARQUIVO);
    printf("Aluno com matrícula %d removido com sucesso!\n", matricula);
}

void alterarCodigoCurso(struct Aluno* head, int matriculaAntiga, int matriculaNova) {
    struct Aluno* temp = head;

    while (temp != NULL) {
        if (temp->matriculaAluno == matriculaAntiga) {
            temp->matriculaAluno = matriculaNova;
            saveAlunosToCSV(head, ARQUIVO);
            printf("Matrícula alterada com sucesso!\n");
            return;
        }
        temp = temp->seguinte;
    }
    printf("Aluno com matrícula %d não encontrado!\n", matriculaAntiga);
}

void printAlunoPorMatricula(struct Aluno* head, int matricula) {
    while (head != NULL) {
        if (head->matriculaAluno == matricula) {
            printf("\nDados do aluno com matrícula %d:\n", matricula);
            printf(" Nome: %s\n", head->nomeAluno);
            printf(" Sobrenome: %s\n", head->sobrenomeAluno);
            printf(" Data de Nascimento: %s\n", head->dataNascimentoAluno);
            printf(" Matrícula: %d\n\n", head->matriculaAluno);
            return;
        }
        head = head->seguinte;
    }
    printf("Aluno com matrícula %d não encontrado!\n", matricula);
}

int main() {
    struct Aluno* alunoHead = NULL;
    char opcao;

    do {
        printf("\n-- SAURUS CADASTRO DE ALUNOS --\n");
        printf("  MENU:\n");
        printf("  1. Cadastrar Aluno\n");
        printf("  2. Exibir Lista de Alunos\n");
        printf("  3. Alterar Matrícula de Aluno\n");
        printf("  4. Eliminar Aluno\n");
        printf("  5. Buscar Aluno por Matrícula\n");
        printf("  6. Sair\n");
        printf("  Escolha uma opção: ");
        scanf(" %c", &opcao);

        switch(opcao) {
            case '1':
                cadastrarAluno(&alunoHead);
                break;
            case '2':
                printf("\n Lista de alunos:\n");
                printAlunos(&alunoHead);
                break;
            case '3': {
                int matriculaAntiga, matriculaNova;
                printf("Digite a matrícula atual do aluno: ");
                scanf("%d", &matriculaAntiga);
                printf("Digite a nova matrícula do aluno: ");
                scanf("%d", &matriculaNova);
                alterarCodigoCurso(alunoHead, matriculaAntiga, matriculaNova);
                break;
            }
            case '4': {
                int matricula;
                printf("Digite a matrícula do aluno a ser removida: ");
                scanf("%d", &matricula);
                eliminarAluno(&alunoHead, matricula);
                break;
            }
            case '5': {
                int matricula;
                printf("Digite a matrícula do aluno: ");
                scanf("%d", &matricula);
                printAlunoPorMatricula(alunoHead, matricula);
                break;
            }
            case '6':
                printf("Saindo do programa...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != '6');

    freeAlunos(alunoHead);

    return 0;
}
