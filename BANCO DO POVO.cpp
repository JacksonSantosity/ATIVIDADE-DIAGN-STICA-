#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVOCONTAS "contas.dat"
#define SENHADOADM "1020"

typedef struct {
    int numero;
    char titular[50];
    float saldo;
} Conta;

int gerarNumeroConta() {
    FILE *f = fopen(ARQUIVOCONTAS, "rb");
    if (f == NULL) return 1;

    Conta c;
    int maior = 0;

    while (fread(&c, sizeof(Conta), 1, f) == 1) {
        if (c.numero > maior) maior = c.numero;
    }

    fclose(f);
    return maior + 1;
}

int buscarConta(int numero, Conta *resultado) {
    FILE *f = fopen(ARQUIVOCONTAS, "rb");
    if (f == NULL) return 0;

    Conta c;

    while (fread(&c, sizeof(Conta), 1, f) == 1) {
        if (c.numero == numero) {
            *resultado = c;
            fclose(f);
            return 1; 
        }
    }

    fclose(f);
    return 0; 
}

int salvarConta(Conta conta) {
    FILE *f = fopen(ARQUIVOCONTAS, "rb+");
    if (f == NULL) return 0;

    Conta c;

    while (fread(&c, sizeof(Conta), 1, f) == 1) {
        if (c.numero == conta.numero) {
            fseek(f, -sizeof(Conta), SEEK_CUR);
            fwrite(&conta, sizeof(Conta), 1, f);
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

void criarConta() {
    Conta nova;
    nova.numero = gerarNumeroConta();
    nova.saldo = 0;

    printf("Digite o nome do titular: ");
    getchar();
    fgets(nova.titular, 50, stdin);
    nova.titular[strcspn(nova.titular, "\n")] = '\0';

    FILE *f = fopen(ARQUIVOCONTAS, "ab");
    fwrite(&nova, sizeof(Conta), 1, f);
    fclose(f);

    printf("Conta criada com sucesso! Numero: %d\n", nova.numero);
}

void extrato() {
    int numero;
    printf("Digite o numero da conta: ");
    scanf("%d", &numero);

    Conta c;
    int achou = buscarConta(numero, &c);

    if (!achou) {
        printf("Conta nao encontrada!\n");
        return;
    }

    printf("\n--- EXTRATO ---\n");
    printf("TITULAR: %s\n", c.titular);
    printf("NUMERO: %d\n", c.numero);
    printf("SALDO: R$ %.2f\n", c.saldo);
}

void deposito() {
    int numero;
    float valor;

    printf("Numero da conta: ");
    scanf("%d", &numero);

    Conta c;
    int achou = buscarConta(numero, &c);

    if (!achou) {
        printf("Conta nao encontrada!\n");
        return;
    }

    printf("Valor do deposito: ");
    scanf("%f", &valor);

    c.saldo += valor;
    salvarConta(c);

    printf("Deposito realizado! Novo saldo: %.2f\n", c.saldo);
}

void saque() {
    int numero;
    float valor;

    printf("Numero da conta: ");
    scanf("%d", &numero);

    Conta c;
    int achou = buscarConta(numero, &c);

    if (!achou) {
        printf("Conta nao encontrada!\n");
        return;
    }

    printf("Valor do saque: ");
    scanf("%f", &valor);

    if (valor > c.saldo) {
        printf("Saldo insuficiente!\n");
        return;
    }

    c.saldo -= valor;
    salvarConta(c);

    printf("Saque realizado! Novo saldo: %.2f\n", c.saldo);
}

void transferencia() {
    int nOrig, nDest;
    float valor;

    printf("Conta de origem: ");
    scanf("%d", &nOrig);
    printf("Conta de destino: ");
    scanf("%d", &nDest);
    printf("Valor: ");
    scanf("%f", &valor);

    Conta origem, destino;

    int achou1 = buscarConta(nOrig, &origem);
    int achou2 = buscarConta(nDest, &destino);

    if (!achou1 || !achou2) {
        printf("Uma ou ambas as contas nao foram encontradas!\n");
        return;
    }

    if (valor > origem.saldo) {
        printf("Saldo insuficiente!\n");
        return;
    }

    origem.saldo -= valor;
    destino.saldo += valor;

    salvarConta(origem);
    salvarConta(destino);

    printf("Transferencia realizada com sucesso!\n");
}

void listarContas() {
    char senha[20];
    printf("Digite a senha de administrador: ");
    scanf("%s", senha);

    if (strcmp(senha, SENHADOADM) != 0) {
        printf("Senha incorreta!\n");
        return;
    }

    FILE *f = fopen(ARQUIVOCONTAS, "rb");
    if (f == NULL) {
        printf("Nenhuma conta cadastrada.\n");
        return;
    }

    Conta c;
    printf("\n--- LISTA DE CONTAS ---\n");

    while (fread(&c, sizeof(Conta), 1, f) == 1) {
        printf("Conta: %d | Titular: %s | Saldo: %.2f\n", 
               c.numero, c.titular, c.saldo);
    }

    fclose(f);
}

int main() {
    int opc;

    do {
      	printf("\n--- BANCO DO POVO ---\n");
        printf("\n--- MENU ---\n");
        printf("1. CRIAR UMA NOVA CONTA\n");
        printf("2. VER EXTRATO\n");
        printf("3. FAZER UM DEPOSITO\n");
        printf("4. SACAR UM VALOR\n");
        printf("5. FAZER UMA TRANSFERENCIA\n");
        printf("6. VER TODAS AS CONTAS\n");
        printf("7. SAIR\n");
        printf("Opcao: ");
        scanf("%d", &opc);

        switch(opc) {
            case 1: criarConta(); break;
            case 2: extrato(); break;
            case 3: deposito(); break;
            case 4: saque(); break;
            case 5: transferencia(); break;
            case 6: listarContas(); break;
            case 7: printf("Saindo... Obrigado Por Utilizar Nossos Servicos!\n"); break;
            default: printf("Opcao invalida!\n");
        }

    } while (opc != 7);

    return 0;
}
