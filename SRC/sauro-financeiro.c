#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINHA 1024

typedef struct {
    char nomeCurso[20];
    int codigoCurso;
    float mensalidade;
    char dataVencimento[20]; // Adiciona data de vencimento
} Curso;

typedef struct {
    char nome[20];
    char sobrenome[20];
    char dataNascimento[20];
    int matricula;
    float valorPago;
    char dataPagamento[20]; // Adiciona data de pagamento
    char statusPagamento[10]; // Adiciona status do pagamento
} Aluno;

typedef struct {
    int matricula;
    char nomeCurso[20];
    int codigoCurso;
} MatriculaCurso;

typedef struct {
    int matricula;
    char nomeCurso[20];
    int codigoCurso;
    char adimplencia[15];
} Financeiro;

void calcularAdimplencia(Aluno alunos[], int alunoQtde, Curso cursos[], int cursoQtde, MatriculaCurso matriculasCursos[], int matriculaCursoQtde, Financeiro financeiros[], int *financeiroQtde) {
    for (int i = 0; i < matriculaCursoQtde; i++) {
        int matricula = matriculasCursos[i].matricula;
        int codigoCurso = matriculasCursos[i].codigoCurso;

        Aluno *aluno = NULL;
        Curso *curso = NULL;

        for (int j = 0; j < alunoQtde; j++) {
            if (alunos[j].matricula == matricula) {
                aluno = &alunos[j];
                break;
            }
        }

        for (int j = 0; j < cursoQtde; j++) {
            if (cursos[j].codigoCurso == codigoCurso) {
                curso = &cursos[j];
                break;
            }
        }

        if (aluno && curso) {
            Financeiro fin;
            fin.matricula = aluno->matricula;
            strcpy(fin.nomeCurso, curso->nomeCurso);
            fin.codigoCurso = curso->codigoCurso;

            // Adimplência com base no status de pagamento
            if (strcmp(aluno->statusPagamento, "Pago") == 0 && aluno->valorPago >= curso->mensalidade) {
                strcpy(fin.adimplencia, "Adimplente");
            } else {
                strcpy(fin.adimplencia, "Inadimplente");
            }

            // Imprimir no console
            printf("Matricula: %d, NomeCurso: %s, CodigoCurso: %d, Adimplencia: %s\n",
                   fin.matricula, fin.nomeCurso, fin.codigoCurso, fin.adimplencia);

            financeiros[*financeiroQtde] = fin;
            (*financeiroQtde)++;
        }
    }
}

void saveFinanceiroToCSV(Financeiro financeiros[], int financeiroQtde, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Não foi possível abrir o arquivo");
        return;
    }

    fprintf(file, "matricula;nomeCurso;codigoCurso;adimplencia\n");

    for (int i = 0; i < financeiroQtde; i++) {
        fprintf(file, "%d;%s;%d;%s\n", financeiros[i].matricula, financeiros[i].nomeCurso, financeiros[i].codigoCurso, financeiros[i].adimplencia);
    }

    fclose(file);
    printf("Dados salvos em %s\n", filename);
}

void verificarVencimento(Curso cursos[], int cursoQtde, char *dataAtual) {
    struct tm dataAtualTm = {0};
    strptime(dataAtual, "%d/%m/%Y", &dataAtualTm);
    time_t dataAtualTime = mktime(&dataAtualTm);

    for (int i = 0; i < cursoQtde; i++) {
        struct tm dataVencimentoTm = {0};
        strptime(cursos[i].dataVencimento, "%d/%m/%Y", &dataVencimentoTm);
        time_t dataVencimentoTime = mktime(&dataVencimentoTm);

        double diff = difftime(dataVencimentoTime, dataAtualTime) / (60 * 60 * 24);

        if (diff <= 5) {
            printf("Curso %s está próximo do vencimento!\n", cursos[i].nomeCurso);
        } else {
            printf("Faltam mais de 5 dias para o vencimento do curso %s.\n", cursos[i].nomeCurso);
        }
    }
}

void registrarPagamento(Aluno alunos[], int alunoQtde) {
    int matricula;
    char status[10];
    printf("Digite a matrícula do aluno: ");
    scanf("%d", &matricula);
    printf("A mensalidade foi paga? (S/N): ");
    scanf("%s", status);

    for (int i = 0; i < alunoQtde; i++) {
        if (alunos[i].matricula == matricula) {
            if (strcmp(status, "S") == 0) {
                strcpy(alunos[i].statusPagamento, "Pago");
            } else {
                strcpy(alunos[i].statusPagamento, "Não Pago");
            }
            break;
        }
    }
}

int main() {
    FILE *alunosArquivo = fopen("alunos.csv", "r");
    FILE *cursosArquivo = fopen("cursos.csv", "r");
    FILE *cursosAlunosArquivo = fopen("cursos-alunos.csv", "r");

    if (!alunosArquivo || !cursosArquivo || !cursosAlunosArquivo) {
        printf("Erro ao abrir arquivos!\n");
        return 1;
    }

    char linha[MAX_LINHA];
    Aluno alunos[100];
    Curso cursos[100];
    MatriculaCurso matriculasCursos[1000];
    Financeiro financeiros[1000];
    int alunoQtde = 0;
    int cursoQtde = 0;
    int matriculaCursoQtde = 0;
    int financeiroQtde = 0;

    // Lê alunos.csv
    fgets(linha, MAX_LINHA, alunosArquivo);
    while (fgets(linha, MAX_LINHA, alunosArquivo)) {
        sscanf(linha, "%[^;];%[^;];%[^;];%d;%f;%s", alunos[alunoQtde].nome, alunos[alunoQtde].sobrenome, alunos[alunoQtde].dataNascimento, &alunos[alunoQtde].matricula, &alunos[alunoQtde].valorPago, alunos[alunoQtde].dataPagamento);
        strcpy(alunos[alunoQtde].statusPagamento, "Não Pago"); // Inicializa como não pago
        alunoQtde++;
    }

    // Lê cursos.csv
    fgets(linha, MAX_LINHA, cursosArquivo);
    while (fgets(linha, MAX_LINHA, cursosArquivo)) {
        sscanf(linha, "%[^;];%d;%f;%s", cursos[cursoQtde].nomeCurso, &cursos[cursoQtde].codigoCurso, &cursos[cursoQtde].mensalidade, cursos[cursoQtde].dataVencimento);
        cursoQtde++;
    }

    // Lê cursos-alunos.csv
    fgets(linha, MAX_LINHA, cursosAlunosArquivo);
    while (fgets(linha, MAX_LINHA, cursosAlunosArquivo)) {
        sscanf(linha, "%d;%[^;];%d", &matriculasCursos[matriculaCursoQtde].matricula, matriculasCursos[matriculaCursoQtde].nomeCurso, &matriculasCursos[matriculaCursoQtde].codigoCurso);
        matriculaCursoQtde++;
    }

    int opcao;
    char dataAtual[20];
    printf("Digite a data de hoje (dd/mm/yyyy): ");
    scanf("%s", dataAtual);

    do {
        printf("Menu:\n");
        printf("1. Verificar vencimento dos cursos\n");
        printf("2. Calcular adimplência dos alunos\n");
        printf("3. Registrar pagamento\n");
        printf("4. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            verificarVencimento(cursos, cursoQtde, dataAtual);
        } else if (opcao == 2) {
            calcularAdimplencia(alunos, alunoQtde, cursos, cursoQtde, matriculasCursos, matriculaCursoQtde, financeiros, &financeiroQtde);
            saveFinanceiroToCSV(financeiros, financeiroQtde, "financeiro.csv");
        } else if (opcao == 3) {
            registrarPagamento(alunos, alunoQtde);
        }
    } while (opcao != 4);

    fclose(alunosArquivo);
    fclose(cursosArquivo);
    fclose(cursosAlunosArquivo);

    printf("Saindo do programa...\n");
    return 0;
}
