#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 20
#define MAX_LINHA 1024

typedef struct {
    char nome[MAX_NOME];
    char sobrenome[MAX_NOME];
    char dataNascimento[MAX_NOME];
    int matricula;
} Aluno;

typedef struct {
    char nomeCurso[MAX_NOME];
    int codigoCurso;
    char dataVencimento[MAX_NOME]; // Novo campo para data de vencimento
} Curso;

typedef struct {
    int matricula;
    char nomeCurso[MAX_NOME];
    int codigoCurso;
} MatriculaCurso;

void cadastrarAlunoNoCurso(FILE *saida, Aluno aluno, Curso curso) {
    fprintf(saida, "%s;%s;%s;%d;%s;%d;%s\n",
            aluno.nome,
            aluno.sobrenome,
            aluno.dataNascimento,
            aluno.matricula,
            curso.nomeCurso,
            curso.codigoCurso,
            curso.dataVencimento); // Adiciona data de vencimento
}

void bubbleSort(Aluno arr[], int n) {
    int i, j;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (arr[j].matricula > arr[j+1].matricula) {
                Aluno temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

void listarMatriculas(Aluno alunos[], int alunoQtde) {
    printf("Matrículas disponíveis:\n");
    for (int i = 0; i < alunoQtde; i++) {
        printf("%d\n", alunos[i].matricula);
    }
}

void listarCursos(Curso cursos[], int cursoQtde) {
    printf("Cursos disponíveis:\n");
    for (int i = 0; i < cursoQtde; i++) {
        printf("Código: %d, Nome: %s, Data de Vencimento: %s\n", cursos[i].codigoCurso, cursos[i].nomeCurso, cursos[i].dataVencimento);
    }
}

int compararMatriculas(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void pesquisarCursosPorMatricula(int matricula, MatriculaCurso matriculasCursos[], int matriculaCursoQtde) {
    int left = 0, right = matriculaCursoQtde - 1;
    int encontrado = 0;

    printf("Cursos matriculados pelo aluno de matrícula %d:\n", matricula);
    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (matriculasCursos[mid].matricula == matricula) {
            encontrado = 1;
            // Encontrou um registro, agora verifica para frente e para trás
            int i = mid;
            while (i >= 0 && matriculasCursos[i].matricula == matricula) {
                printf("Código: %d, Nome: %s\n", matriculasCursos[i].codigoCurso, matriculasCursos[i].nomeCurso);
                i--;
            }
            i = mid + 1;
            while (i < matriculaCursoQtde && matriculasCursos[i].matricula == matricula) {
                printf("Código: %d, Nome: %s\n", matriculasCursos[i].codigoCurso, matriculasCursos[i].nomeCurso);
                i++;
            }
            break;
        } else if (matriculasCursos[mid].matricula < matricula) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    if (!encontrado) {
        printf("Nenhum curso encontrado para a matrícula %d.\n", matricula);
    }
}

int main() {
    FILE *alunosArquivo = fopen("alunos.csv", "r");
    FILE *cursosArquivo = fopen("cursos.csv", "r");
    FILE *saidaArquivo = fopen("cursos-alunos.csv", "a");
    FILE *cursosAlunosArquivo = fopen("cursos-alunos.csv", "r");

    if (!alunosArquivo || !cursosArquivo || !saidaArquivo) {
        printf("Erro ao abrir arquivos!\n");
        return 1;
    }

    char linha[MAX_LINHA];
    Aluno alunos[100];
    Curso cursos[100];
    MatriculaCurso matriculasCursos[1000];
    int alunoQtde = 0;
    int cursoQtde = 0;
    int matriculaCursoQtde = 0;

    // Lê alunos.csv
    fgets(linha, MAX_LINHA, alunosArquivo);
    while (fgets(linha, MAX_LINHA, alunosArquivo)) {
        sscanf(linha, "%[^;];%[^;];%[^;];%d", alunos[alunoQtde].nome, alunos[alunoQtde].sobrenome, alunos[alunoQtde].dataNascimento, &alunos[alunoQtde].matricula);
        alunoQtde++;
    }

    // Ordena as matrículas usando BubbleSort
    bubbleSort(alunos, alunoQtde);

    // Lê cursos.csv
    fgets(linha, MAX_LINHA, cursosArquivo); // Pula o cabeçalho
    while (fgets(linha, MAX_LINHA, cursosArquivo)) {
        sscanf(linha, "%[^;];%d;%s", cursos[cursoQtde].nomeCurso, &cursos[cursoQtde].codigoCurso, cursos[cursoQtde].dataVencimento); // Adiciona data de vencimento
        cursoQtde++;
    }

    // Lê cursos-alunos.csv
    if (cursosAlunosArquivo) {
        fgets(linha, MAX_LINHA, cursosAlunosArquivo); // Pula o cabeçalho
        while (fgets(linha, MAX_LINHA, cursosAlunosArquivo)) {
            sscanf(linha, "%*[^;];%*[^;];%*[^;];%d;%[^;];%d", &matriculasCursos[matriculaCursoQtde].matricula, matriculasCursos[matriculaCursoQtde].nomeCurso, &matriculasCursos[matriculaCursoQtde].codigoCurso);
            matriculaCursoQtde++;
        }
        fclose(cursosAlunosArquivo);

        // Ordena as matriculasCursos para permitir a pesquisa binária
        qsort(matriculasCursos, matriculaCursoQtde, sizeof(MatriculaCurso), compararMatriculas);
    }

    // Escreve cabeçalho no arquivo de saída se o arquivo está vazio
    if (ftell(saidaArquivo) == 0) {
        fprintf(saidaArquivo, "Nome;Sobrenome;DataDeNascimento;Matricula;NomeCurso;CodigoCurso;DataVencimento\n");
    }

    int opcao;
    do {
        printf("Menu:\n");
        printf("1. Cadastrar aluno em curso\n");
        printf("2. Pesquisar cursos de um aluno\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            listarMatriculas(alunos, alunoQtde);

            int matricula, codigoCurso;
            printf("Digite a matrícula do aluno: ");
            scanf("%d", &matricula);

            listarCursos(cursos, cursoQtde);

            printf("Digite o código do curso: ");
            scanf("%d", &codigoCurso);

            Aluno alunoEscolhido;
            Curso cursoEscolhido;
            int alunoEncontrado = 0, cursoEncontrado = 0;

            for (int i = 0; i < alunoQtde; i++) {
                if (alunos[i].matricula == matricula) {
                    alunoEscolhido = alunos[i];
                    alunoEncontrado = 1;
                    break;
                }
            }

            for (int i = 0; i < cursoQtde; i++) {
                if (cursos[i].codigoCurso == codigoCurso) {
                    cursoEscolhido = cursos[i];
                    cursoEncontrado = 1;
                    break;
                }
            }

            if (alunoEncontrado && cursoEncontrado) {
                cadastrarAlunoNoCurso(saidaArquivo, alunoEscolhido, cursoEscolhido);
                printf("Aluno cadastrado com sucesso!\n");
            } else {
                if (!alunoEncontrado) {
                    printf("Matrícula de aluno não encontrada!\n");
                }
                if (!cursoEncontrado) {
                    printf("Código de curso não encontrado!\n");
                }
            }
        } else if (opcao == 2) {
            listarMatriculas(alunos, alunoQtde);

            int matricula;
            printf("Digite a matrícula do aluno: ");
            scanf("%d", &matricula);

            pesquisarCursosPorMatricula(matricula, matriculasCursos, matriculaCursoQtde);
        }
    } while (opcao != 3);

    fclose(alunosArquivo);
    fclose(cursosArquivo);
    fclose(saidaArquivo);

    printf("Saindo do programa...\n");
    return 0;
}
