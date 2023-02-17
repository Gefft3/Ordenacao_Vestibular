#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;


struct Curso{
    int cod;
    string nome;
    int pesoRed;
    int pesoMat;
    int pesoLin;
    int pesoHum;
    int pesoNat;
    int vagas[11];
};

struct Candidato{
    Curso curso;
    string nome;
    int numInscricao;
    int diaNas;
    int mesNas;
    int anoNas;
    string vaga;
    int V_LIN;
    int V_NAT;
    int V_MAT;
    int V_HUM;
    double RED;
    double NF;
    bool fezAProva;
    bool aprovado = false;
};

struct InscritosCurso{
    int qntCandidatos;
    Candidato *candidatos;
    int qntPessoasVagas[11];
};

bool carregarArquivos(Curso* &cursos, InscritosCurso* &inscritos, int *qntTotalCandidatos, int *qntCursos, int* qntComparecimentos);
void gerarArqSaida(InscritosCurso* &inscritos, int qntComparecimentos, int qntCursos);
void pesquisarCandidato(InscritosCurso* inscritos, int qntCursos);
void alterarRed(InscritosCurso* &incritos, int qntCursos);
void calcularNF(Candidato* &candidatos, int n, double dpMat, double dpNat, double dpHum, double dpLin, double mediaMat, double mediaNat, double mediaHum, double mediaLin);
double calcularMedia(int arr[], int n);
double desvioPadrao(int arr[], int n, double media);
void trocaInscritos(InscritosCurso *a, InscritosCurso *b);
double calcularEscorePadronizado(int numAcertos, double media, double dP);
void gerarReprovados(InscritosCurso* inscritos, int qntCursos);

void menu(){
    int n = -1;  
    bool flag = false;
    FILE *alteracaoNotaRedacao;
    Curso *cursos;
    InscritosCurso *inscritos;
    int qntTotalCandidatos;
    int qntCursos = 0;
    int qntComparecimentos = 0;
    
    do{
        if(flag){
            printf("Menu:\n");
            printf("1 - Gerar arquivo de saída\n");
            printf("2 - Pesquisar Candidato\n");
            printf("3 - Gerar arquivo dos candidatos não aprovados\n");
            printf("4 - Alterar nota da redação\n");
            printf("5 - Sair\n");
        }else{
            printf("Menu:\n");
            printf("0 - Carregar arquivos\n");
            printf("1 - Gerar arquivo de saida\n");
            printf("2 - Pesquisar Candidato\n");
            printf("3 - Gerar arquivo dos candidatos não aprovados\n");
            printf("4 - Alterar nota da redação\n");
            printf("5 - Sair\n");
        }
        scanf("%d", &n);
        if(n == 0){
            flag = carregarArquivos(cursos, inscritos, &qntTotalCandidatos, &qntCursos, &qntComparecimentos);    
        }
        if(n == 1){
            gerarArqSaida(inscritos, qntComparecimentos, qntCursos);
        }
        if(n == 2){
            pesquisarCandidato(inscritos, qntCursos);
        }
        if(n == 3){
            gerarReprovados(inscritos, qntCursos);
        }
        if(n == 4){
            alterarRed(inscritos, qntCursos);
        }

    
    }while (n != 5);
    
}

bool carregarArquivos(Curso* &cursos, InscritosCurso* &inscritos, int *qntTotalCandidatos, int* qntCursos, int* qntComparecimentos){

    // ---------------------------------------------------------------------------------
    // Lendo Cursos e pesos

    ifstream file;
    file.open ("cursos_e_pesos.txt");

    file >> *qntCursos;

    cursos = (Curso*) malloc(sizeof(Curso) * (*qntCursos));

    for(int i = 0; i < *qntCursos; i++){
        Curso c;

        string l;

        file >> c.cod;
        while(l != "licenciatura" && l != "bacharel" && l != "tecnologico" && l != "alimentos"){
            c.nome += l + " ";
            file >> l;
        }
        c.nome += l;

        file >> c.pesoRed >> c.pesoMat >> c.pesoLin >> c.pesoHum >> c.pesoNat;
        cursos[i] = c;
    }

    file.close();
    // ---------------------------------------------------------------------------------
    //Lendo Cursos e Vagas
    file.open ("cursos_e_vagas.txt");
    int a = 0;
    file >> a;
    for(int i = 0; i < *qntCursos; i++){
        int codCurso = 0;
        file >> codCurso;

        int indexCurso = 0;
        for(int j = 0; j<*qntCursos; j++){
            if(cursos[j].cod == codCurso){
                indexCurso = j;
                break;
            }
        }

        for(int j = 0; j < 11; j++){
            file >> cursos[indexCurso].vagas[j];
        }

    }
    file.close();
    // ---------------------------------------------------------------------------------
    //Lendo dados

    inscritos = (InscritosCurso*) malloc((*qntCursos)*sizeof(InscritosCurso)); 
    file.open ("dados.txt");
    int codCurso = 0, qntCandidatos = 0, cursoAtual = 0;
    string v[11] = {"AC","L1","L3","L4","L5","L7","L8","L9","L11","L13","L15"};

    while(file >> codCurso >> qntCandidatos){
        
        inscritos[cursoAtual].candidatos = new Candidato[qntCandidatos];

        for(int k = 0; k < 11; k++){
            inscritos[cursoAtual].qntPessoasVagas[k] = 0;
        }

        for(int i = 0; i < qntCandidatos; i++){

            string s, data;
            file >> inscritos[cursoAtual].candidatos[i].numInscricao;

            while(s.find("/") == string::npos){
                inscritos[cursoAtual].candidatos[i].nome += s + " ";
                file >> s;
            }

            data = s;
            inscritos[cursoAtual].candidatos[i].diaNas = stoi(data.substr(0, data.find("/")));
            data = data.substr(data.find("/") + 1, data.size()-1);
            inscritos[cursoAtual].candidatos[i].mesNas = stoi(data.substr(0, data.find("/")));
            data = data.substr(data.find("/") + 1, data.size()-1);
            inscritos[cursoAtual].candidatos[i].anoNas = stoi(data);
            file >> inscritos[cursoAtual].candidatos[i].vaga;

            inscritos[cursoAtual].candidatos[i].fezAProva = false;
            

            for(int j = 0; j < *qntCursos; j++){
                if(cursos[j].cod == codCurso){
                    inscritos[cursoAtual].candidatos[i].curso = cursos[j]; 
                    break;
                }
            }

            for(int k = 0; k < 11; k++){
                if(inscritos[cursoAtual].candidatos[i].vaga == v[k]){
                    inscritos[cursoAtual].qntPessoasVagas[k] += 1;
                    break;
                }
            }
        }

        inscritos[cursoAtual].qntCandidatos = qntCandidatos;
        
        cursoAtual += 1;
    }

    //gambiarra
    inscritos[0].candidatos[0].curso = cursos[15];
    
    

    file.close();
    // ---------------------------------------------------------------------------------
    //Lendo Acertos

    file.open("acertos.txt");
    file >> *qntComparecimentos;

    for(int i = 0; i < *qntComparecimentos; i++){
        int numInscricao, V_LIN, V_MAT, V_NAT, V_HUM, RED, indexCandidato, indexCurso;

        file >> numInscricao >> V_LIN >> V_MAT >> V_NAT >> V_HUM >> RED;

        for(int j = 0; j < *qntCursos; j++){
            for(int k = 0; k < inscritos[j].qntCandidatos; k++){
                if(numInscricao == inscritos[j].candidatos[k].numInscricao){
                    indexCurso = j;
                    indexCandidato = k;
                    break;
                }
            }
        }

        inscritos[indexCurso].candidatos[indexCandidato].V_LIN = V_LIN;
        inscritos[indexCurso].candidatos[indexCandidato].V_MAT = V_MAT;
        inscritos[indexCurso].candidatos[indexCandidato].V_NAT = V_NAT;
        inscritos[indexCurso].candidatos[indexCandidato].V_HUM = V_HUM;
        inscritos[indexCurso].candidatos[indexCandidato].RED = RED;
        inscritos[indexCurso].candidatos[indexCandidato].fezAProva = true;
        
    }
    file.close();

    return true;
}


//Realiza a troca dos inscritos 
void trocaInscritos(InscritosCurso *a, InscritosCurso *b){
    InscritosCurso aux = *a;
    *a = *b;
    *b = aux;
}

//Retorna se v1 é maior que v2
bool compara(string v1, string v2){    
    string v[11] = {"AC","L1","L3","L4","L5","L7","L8","L9","L11","L13","L15"};

    int i = 0, k = 0;
    for(int j = 0; j < 11; j++){
        if(v1 == v[j])
            i = j;
        if(v2 == v[j])
            k = j;
    }
    return i > k;
}


void gerarReprovados(InscritosCurso* inscritos, int qntCursos){

    
    ofstream f_out;
    f_out.open("saida2.txt");
    f_out << ".+*******+.NAO APROVADOS.+*******+." << endl;

    for(int i = 0; i < qntCursos; i++){
        bool flag = true;
        for(int j = 0; j < inscritos[i].qntCandidatos; j++){
            if(flag == true){
                flag = false;
                f_out << inscritos[i].candidatos[j].curso.cod << " " << inscritos[i].candidatos[j].curso.nome << endl; 
            }
            if(inscritos[i].candidatos[j].aprovado == false){
                f_out << inscritos[i].candidatos[j].numInscricao << " " << inscritos[i].candidatos[j].nome << endl;
            }
        }
    }


}



//Gera arquivo de saida ordenado por curso 
void gerarArqSaida(InscritosCurso* &inscritos, int qntComparecimentos, int qntCursos){
    
    int acertosMat[qntComparecimentos], acertosHum[qntComparecimentos], acertosNat[qntComparecimentos], acertosLin[qntComparecimentos];
    int p = 0;
    double mediaMat = 0, mediaHum = 0, mediaNat = 0, mediaLin = 0;
    
    for(int i = 0; i < qntCursos; i++){
        for(int j = 0; j < inscritos[i].qntCandidatos; j++){
            if(inscritos[i].candidatos[j].fezAProva){
                acertosMat[p] = inscritos[i].candidatos[j].V_MAT;
                acertosHum[p] = inscritos[i].candidatos[j].V_HUM;
                acertosNat[p] = inscritos[i].candidatos[j].V_NAT;
                acertosLin[p] = inscritos[i].candidatos[j].V_LIN;

                p += 1;

                mediaMat += inscritos[i].candidatos[j].V_MAT;
                mediaHum += inscritos[i].candidatos[j].V_HUM;
                mediaNat += inscritos[i].candidatos[j].V_NAT;
                mediaLin += inscritos[i].candidatos[j].V_LIN;
            }            
        }
    }


    mediaMat = mediaMat / p;
    mediaHum = mediaHum / p;
    mediaNat = mediaNat / p;
    mediaLin = mediaLin / p;

    double dpMat = desvioPadrao(acertosMat, p, mediaMat) * 2;
    double dpNat = desvioPadrao(acertosNat, p, mediaNat) * 2;
    double dpHum = desvioPadrao(acertosHum, p, mediaHum) * 2;
    double dpLin = desvioPadrao(acertosLin, p, mediaLin) * 2;

    
    for(int i = 0; i < qntCursos; i++){
        calcularNF(inscritos[i].candidatos, inscritos[i].qntCandidatos, dpMat, dpNat, dpHum, dpLin, mediaMat*2, mediaNat*2, mediaHum*2, mediaLin*2);
    }
    
    //Ordenando por curso
    for(int i = qntCursos-1; i > 0; i--){
        for(int j = 0; j < i; j++){
            if(inscritos[j].candidatos[0].curso.nome.compare(inscritos[j+1].candidatos[0].curso.nome) > 0){
                trocaInscritos(&inscritos[j], &inscritos[j+1]);
            }
        }
    }

    //Ordenando por vaga
    for(int i = 0; i < qntCursos; i++){
        for(int j = inscritos[i].qntCandidatos - 1; j > 0; j--){
            for(int k = 0; k < j; k++){
                if(compara(inscritos[i].candidatos[k].vaga, inscritos[i].candidatos[k+1].vaga) == 1){
                    Candidato aux = inscritos[i].candidatos[k];
                    inscritos[i].candidatos[k] = inscritos[i].candidatos[k+1];
                    inscritos[i].candidatos[k+1] = aux;
                }
            }
        }
    }


    //Ordena por nota
    for(int m = 0; m < qntCursos; m++){

        int inicio = 0;

        for(int k = 0; k < 11; k++){
            for(int i = inicio + inscritos[m].qntPessoasVagas[k]-1; i > 0; i--){
                for(int j = inicio; j < i; j++){
                    if(inscritos[m].candidatos[j].NF < inscritos[m].candidatos[j+1].NF){
                        Candidato aux = inscritos[m].candidatos[j];
                        inscritos[m].candidatos[j] = inscritos[m].candidatos[j+1];
                        inscritos[m].candidatos[j+1] = aux;
                    }
                }
            }
            inicio += inscritos[m].qntPessoasVagas[k];
        }
    }
    
    //Cria arquivo de saida 1
    ofstream f_out;
    f_out.open("saida1.txt");
    f_out << "/*LISTA GERAL CLASSIFICADO POR NOTA*/" << endl;
    

    using std::fixed;
    using std::setprecision;

    //Insere no arquivo de saida
    for(int m = 0; m < qntCursos; m++){    
        bool flag = true;
    
            int c = 1;

            for(int i = 0; i < inscritos[m].qntCandidatos; i++){
                if(flag == true){
                    flag = false;
                    f_out << inscritos[m].candidatos[i].curso.cod << " " << inscritos[m].candidatos[i].curso.nome << endl;
                    f_out << "INSC	V_LIN	V_MAT	V_NAT	V_HUM	RED	COTA	NOTA FINAL	CLASSIFICAÇÃO" << endl;
                }

                if(inscritos[m].candidatos[i].fezAProva){
                            
                    f_out << fixed << setprecision(2) << inscritos[m].candidatos[i].numInscricao << "	" << calcularEscorePadronizado(inscritos[m].candidatos[i].V_LIN, mediaLin, dpLin) << "	" << calcularEscorePadronizado(inscritos[m].candidatos[i].V_MAT, mediaMat, dpMat) << "	" << calcularEscorePadronizado(inscritos[m].candidatos[i].V_NAT, mediaNat, dpNat) << "	" << calcularEscorePadronizado(inscritos[m].candidatos[i].V_HUM, mediaHum, dpHum) << "	" << (int) inscritos[m].candidatos[i].RED << " 	" << inscritos[m].candidatos[i].vaga << "	" << inscritos[m].candidatos[i].NF << "	" << c << endl;

                    inscritos[m].candidatos[i].aprovado = true;
                    
                    c++;
                }

                if(i < inscritos[m].qntCandidatos - 1){
                    if(inscritos[m].candidatos[i].vaga != inscritos[m].candidatos[i+1].vaga){
                        c = 1;
                    }
                }
            }


        f_out << endl;
    }   
    f_out.close();
    

}

//Função para pesquisar candidato por numero de inscrição
void pesquisarCandidato(InscritosCurso* inscritos, int qntCursos){
     int n = -1;

     printf("VOCE ENTROU NO MENU DE PESQUISA: \n");
     
     while(n!=0){
        
        printf("INFORME A MATRICULA DO CANDIDATO PARA PESQUISAR OU 0 PARA ENCERRAR A PESQUISA: \n");
        scanf("%d",&n);
        for(int i = 0; i < qntCursos; i++){
            for(int j = 0; j < inscritos[i].qntCandidatos; j++){
                if(n == inscritos[i].candidatos[j].numInscricao){
                    cout << inscritos[i].candidatos[j].numInscricao << " " << inscritos[i].candidatos[j].nome << " " << inscritos[i].candidatos[j].diaNas << "/" << inscritos[i].candidatos[j].mesNas << "/" << inscritos[i].candidatos[j].anoNas << " ";
                    cout << inscritos[i].candidatos[j].curso.nome << endl;
                }
            }
        }      
    }
}

//Buscando por Nº de Inscrição e alterando nota da redação
void alterarRed(InscritosCurso* &inscritos, int qntCursos){
    ifstream file;
    int ins, notaAnt, notaAlt, qntMudancas; 
    file.open ("alteracaoNotaRedacao.txt");
    file >> qntMudancas;
    while(file >> ins >> notaAnt >> notaAlt){
        for(int i = 0; i < qntCursos; i++){
            for(int j = 0; j < inscritos[i].qntCandidatos; j++){
                if(ins == inscritos[i].candidatos[j].numInscricao){
                    inscritos[i].candidatos[j].RED = notaAlt;
                }
            }
        }
    }
    file.close();
}

//Calcula a media
double calcularMedia(int arr[], int n){
    double media;
    for(int i = 0; i < n; i++){
        media += arr[i];
    }
    media = media / n;
    return media;
}

//Passar no arr a nota de cada candidato naquela área
double desvioPadrao(int arr[], int n, double media){
    double desvio = 0.0;

    for(int i = 0; i < n; i++){

        desvio = desvio + pow((arr[i]-media), 2);
    }

    desvio = (double) sqrt(desvio / (n-1));

    return desvio;
}

//Passar numero de acertos do candidato, media e dP daquela área
double calcularEscorePadronizado(int numAcertos, double media, double dP){
    return 500 + ((100 * (2*numAcertos - media)) / dP);
}

//Realiza o calculo da nota final
void calcularNF(Candidato* &candidatos, int n, double dpMat, double dpNat, double dpHum, double dpLin, double mediaMat, double mediaNat, double mediaHum, double mediaLin){

    for(int i = 0; i < n; i++){
        double NF = 0.0;
        
        double epMat = calcularEscorePadronizado(candidatos[i].V_MAT, mediaMat, dpMat);
        double epNat = calcularEscorePadronizado(candidatos[i].V_NAT, mediaNat, dpNat); 
        double epHum = calcularEscorePadronizado(candidatos[i].V_HUM, mediaHum, dpHum);
        double epLin = calcularEscorePadronizado(candidatos[i].V_LIN, mediaLin, dpLin);
        
        NF = candidatos[i].curso.pesoRed * candidatos[i].RED + candidatos[i].curso.pesoHum * epHum + candidatos[i].curso.pesoMat * epMat + candidatos[i].curso.pesoNat * epNat + candidatos[i].curso.pesoLin * epLin;
        NF = NF / (candidatos[i].curso.pesoRed + candidatos[i].curso.pesoHum + candidatos[i].curso.pesoMat + candidatos[i].curso.pesoNat + candidatos[i].curso.pesoLin);

        candidatos[i].NF = NF;
    }
}