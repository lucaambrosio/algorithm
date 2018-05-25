#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#define MAX 65535
typedef unsigned short int16_t;
size_t size;
long long i=0;
long long cont =0;
char *buffer;
char app[MAX];
long long b=0;
FILE *output;

bool isEmpty(FILE *file) { // funzione utilizzata per verificare
    long savedOffset = ftell(file);
    fseek(file, 0, SEEK_END);

    if (ftell(file) == 0) {
        return true;
    }

    fseek(file, savedOffset, SEEK_SET);
    return false;
}

//funzione utilizzata per la scrittura bufferizzata in fase di decompressione
    void bufferwork() {
        if (cont == 99999 || i == size) {

            if (b != 0) {
                for (int c = MAX; c < cont; c++) {
                    // printf("%c",buffer[c]);
                    fputc(buffer[c], output);
                }
            } else {
                for (int c = 0; c < cont; c++) {
                    // printf("%c",buffer[c]);
                    fputc(buffer[c], output);
                }
                b++;
            }
            for (int c = 0; c < MAX; c++) {
                app[c] = buffer[99999 - MAX + c];
            }
            for (int v = 0; v < MAX; v++) {
                buffer[v] = app[v];
            }

            cont = MAX;

        }
    }



void comprimi(unsigned char filedacomprimere[],unsigned char filecompresso[]) {//funzione di compressione
    printf("\a");
    unsigned short zero = 0;
    clock_t begin = clock();//rilevamento del tempo
    long long cont = 0;//numero di volte che scrivo sul file compresso
    long long lookatindexpos = 0;//variabile che parte da zero e viene incrementata e rappresenta la posizione del lopkat
    long long indexpos = 0;//posizine nel buffer
    ///long long i = 0;
    long long searchbuffoff = 0;
    unsigned short loalength = MAX;//lunghezza lokka head buffer
    unsigned short sblength = MAX;//search buffer length
    unsigned short maxlength = 0;//la corrispondenza più lunga
    long long offset = 0;//distanza dal inizio del pattern
    unsigned short foff = 0;//offset della corrispondeza maggiore
    long long posfin = 0;//la posizione del index dopo la corrispondenza trovata
    unsigned char nextchar;//variabile rappresentante del prossimo carattere dopo il pattern ripetuto
    FILE *input; //puntatore al file in input da comprimere
    int corrispondenza = 0;//varibile utilizzata per la conferma di una corrispondenza
    int bitr = 0; //bit risparmiti se funzionasse la scrittura a bit
    input = fopen(filedacomprimere, "rb");
    FILE *output = fopen(filecompresso, "wb");
    if (input == NULL) {
        printf("non esiste il file !!");
    }
    fseek(input, 0, SEEK_END);
    size_t sizeFile = (size_t) ftell(input);
    fseek(input, 0, SEEK_SET);
    unsigned char *buffer = (unsigned char *) malloc(sizeFile * sizeof(unsigned char));
    fread(buffer, sizeof(unsigned char), sizeFile, input);
    //printf("leggo: %s",argv[1]);
    //for (int i = 0; i < sizeFile; i++) {
    //    printf("%c", buffer[i]);
    //}
    //unsigned char *lookbuffer = (unsigned char *) malloc(3 * sizeof(unsigned char));
    //compressore
    if (isEmpty(input)) {
        printf(" file vuoto \n\n");
        exit(0);
    }
    //fputc(0,output);
    //fputc(0,output);
    fwrite(&zero, 2, 1, output);
    fwrite(&zero, 2, 1, output);
    fputc(buffer[0], output);
    cont++;
    //ricerca dei pattern brute - force
    for (indexpos = 1; indexpos < sizeFile; indexpos++) {//ciclo dalla dalla prima alla ultima posione dell'array contenente tutto
        for (searchbuffoff = indexpos - 1;
             searchbuffoff > -1 && searchbuffoff >= indexpos - sblength; searchbuffoff--) {
            offset++;
            while (buffer[searchbuffoff + lookatindexpos] == buffer[indexpos + lookatindexpos]) {
                lookatindexpos++;
                if (lookatindexpos == loalength || indexpos + lookatindexpos == sizeFile - 1) {
                    break;
                }

            }
            if (maxlength < lookatindexpos) {
                corrispondenza = 1;
                nextchar = buffer[indexpos + lookatindexpos];
                maxlength = lookatindexpos;
                foff = offset;
                posfin = indexpos + lookatindexpos;
                if (lookatindexpos == loalength) {
                    lookatindexpos = 0;
                    break;
                }

            }
            lookatindexpos = 0;
        }
        if (!corrispondenza) {
            nextchar = buffer[indexpos];
            foff = 0;
        } else {
            indexpos = posfin;
            corrispondenza = 0;
        }

        //bitr = bitr + 2; //contiene i bit risparimiati se la scrittura a bit funzionasse
        fwrite(&foff, sizeof(unsigned short), 1, output);//scrivo offset
        fwrite(&maxlength, sizeof(unsigned short), 1, output);//scrivo la lunghezza sul file compresso
        fputc(nextchar, output);//scrivo il prossimo carattere
        cont++;
        offset = 0;
        maxlength = 0;
    }
    printf("finish!!\n\n");
    fclose(input);//libero risorse
    //printf("\n%d\n", bitr);
    fclose(output);//libero risorse
    clock_t end = clock();//tempo finale
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;//calcolo il tempo
    printf("secondi : %lf", time_spent);//stampo il tempo
    printf("\a");//suono
}

void decomprimi(unsigned char filecompresso[],unsigned char fileoutput[]){//funzione di decompressione
    clock_t begin = clock();
    unsigned short off;
    unsigned short lunghezza;
    unsigned char next_char;
    FILE *input;
    long long n = 0;
    input = fopen(filecompresso, "rb");
    output = fopen(fileoutput, "wb");
    if (input == NULL) {
        printf("non esiste il file !!");
        exit(0);
    }
    buffer = (char *) malloc(100000 * sizeof(char));
    fseek(input, 0, SEEK_END);
    size = (size_t) ftell(input);
    fseek(input, 0, SEEK_SET);
    for (i = 0; i < size + 5; i = i + 5) {
        fread(&off, 1, 2, input);
        fread(&lunghezza, 1, 2, input);
        fread(&next_char, 1, 1, input);

        for (n = 0; n < lunghezza; n++) {
            bufferwork();
            buffer[cont] = buffer[cont - off];
            if (i != size)
                cont++;

        }
        bufferwork();
        buffer[cont] = next_char;
        if (i != size)
            cont++;
        bufferwork();
    }
    fclose(input);
    fclose(output);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("secondi : %lf",time_spent);
    printf("\nfinish!!!!");
}
int main(int argc, char **args){
    if(strcmp(args[1],"-c")==0){
        comprimi(args[2],args[3]);//chiamo compri
    }else if(strcmp(args[1],"-d")==0){
        decomprimi(args[2],args[3]);
    }else{
        printf("scelta non valida!!!!!");
    }


}












/*
 *

//progetto svolto con gli operatori bit a bit ma succede che in alcuni file l'ultima parte del file ossia l'ultima codifica
//non venga compressa per qeusto la lascio commentata
//ma non riesco a trovare l 'errore in decompressione
#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define MAX 32767
typedef unsigned short int16_t;
#include <string.h>

bool isEmpty(FILE *file){   //fonte stack overflow utilizzato per verificare se il file è vuoto in input
    long savedOffset = ftell(file);
    fseek(file, 0, SEEK_END);

    if (ftell(file) == 0){
        return true;
    }

    fseek(file, savedOffset, SEEK_SET);
    return false;
}
void comprimi(unsigned char filedacomprimere[],unsigned char filecompresso[]){
    int bitposition=0;
    unsigned char buf[19];
    for(int h=0;h<19;h++)
        buf[h]=0;
    int indice=0;
    unsigned short temp;
    clock_t begin = clock();
    long long cont = 0;
    long long lookatindexpos=0;//variabile che parte da zero e viene incrementata e rappresenta la posizione del lopkat
    long long indexpos = 0;//posizine nel buffer
    long long searchbuffoff = 0;
    unsigned short loalength = MAX;
    unsigned short sblength = MAX;
    unsigned short maxlength=0;//la corrispondenza più lunga
    long long offset=0;
    unsigned short foff=0;//offset della corrispondeza maggiore
    long long posfin=0;//la posizione del index dopo la corrispondenza trovata
    unsigned char nextchar;
    FILE *input;
    int corrispondenza=0;
    input = fopen(filedacomprimere, "rb");
    FILE *output=fopen(filecompresso,"wb");
    if(input == NULL){
        printf("non esiste il file !!");
    }
    fseek(input, 0, SEEK_END);
    size_t sizeFile = (size_t) ftell(input);
    fseek(input, 0, SEEK_SET);
    unsigned char *buffer = (unsigned char *) malloc(sizeFile * sizeof(unsigned char));
    fread(buffer, sizeof(unsigned char), sizeFile, input);
    if(isEmpty(input)){
        printf(" file vuoto \n\n");
        exit(0);
    }
    cont++;
    for (indexpos = 0; indexpos < sizeFile; indexpos++) {
        for (searchbuffoff = indexpos - 1;
             searchbuffoff > -1 && searchbuffoff >= indexpos - sblength; searchbuffoff--) {
            offset++;
            while(buffer[searchbuffoff+lookatindexpos]==buffer[indexpos+lookatindexpos] ){
                lookatindexpos++;
                if( lookatindexpos==loalength ||indexpos+lookatindexpos==sizeFile -1){
                    break;
                }

            }
            if(maxlength<lookatindexpos){
                corrispondenza=1;
                nextchar=buffer[indexpos+lookatindexpos];
                maxlength=lookatindexpos;
                foff=offset;
                posfin=indexpos+lookatindexpos;
                if(lookatindexpos==loalength ){
                    lookatindexpos=0;
                    break;
                }

            }
            lookatindexpos=0;
        }
        if(!corrispondenza){
            nextchar=buffer[indexpos];
            foff=0;
        }
        else{
            indexpos=posfin;
            corrispondenza=0;
        }

        printf("\nmax: %d",maxlength);
        temp=maxlength<<(1);    //disegno 1
        temp= temp >>9; //disegno 2
//        printf("bitposition : %d\n",bitposition);
        temp=temp<<1;   //disegno 3
        buf[indice]=buf[indice] | (temp >> (bitposition));  //disegno 4
        bitposition+=7;
        if(bitposition>8) {
            bitposition %= 8;
          //printf("Buf[indice]: %d\n",buf[indice]);
            indice++;
            buf[indice]=(temp << (8-bitposition-1));    //disegno 5
        }
        if(bitposition==8){
            //printf("Buf[indice]: %d\n",buf[indice]);
            indice++;
            bitposition%=8;
        }
        temp=maxlength<<8;
        temp=temp>>8;
        //printf("\ntemp significativo %d\n",temp);//disegno 7
        buf[indice]=buf[indice] | (temp >> (bitposition));
        bitposition+=8;
        //printf("Buf[indice]: %d\n",buf[indice]);
        indice++;
        if(bitposition>8) {
            bitposition %= 8;
            buf[indice]=(temp << (8-bitposition));    //disegno 5
        }
        bitposition%=8;
        printf("off %d",foff);
        temp=foff<<(1);    //disegno 1
        temp= temp >>9; //disegno 2
        temp=temp<<1;   //disegno 3
        buf[indice]=buf[indice] | (temp >> (bitposition));  //disegno 4
        bitposition+=7;
        if(bitposition>8) {
            bitposition %= 8;
            //printf("Buf[indice]: %d\n",buf[indice]);
            indice++;
            buf[indice]=(temp << (8-bitposition-1));    //disegno 5
        }
        if(bitposition==8){
           // printf("Buf[indice]: %d\n",buf[indice]);
            indice++;
            bitposition%=8;
        }
        temp=foff<<8;
        temp=temp>>8;   //disegno 7
        buf[indice]=buf[indice] | (temp >> (bitposition));
        bitposition+=8;
        //printf("Buf[indice]: %d\n",buf[indice]);
        indice++;
        if(bitposition>8) {
            bitposition %= 8;
            buf[indice]=(temp << (8-bitposition));    //disegno 5
        }
        bitposition%=8;

        temp=nextchar;
        printf(" cara %c",temp);
        buf[indice]=buf[indice] | (temp >> (bitposition));
        bitposition+=8;
//        printf("Buf[indice]: %d",buf[indice]);
        indice++;
        if(bitposition>8) {
            bitposition %= 8;
            buf[indice]=(temp << (8-bitposition));    //disegno 5
        }
        if(indice==19 || indexpos+lookatindexpos>=sizeFile-1 ){
            fwrite(buf,sizeof(char),19,output);
            for(int h=0;h<19;h++)
                buf[h]=0;
            indice=0;
        }

        bitposition%=8;
        cont++;
        offset=0;
        maxlength=0;

    }
    printf("finish!!\n\n");
    fclose(input);
    fclose(output);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("secondi : %lf",time_spent);
}

void decomprimi(unsigned char filecompresso[],unsigned char fileoutput[]){
    char app[MAX];
    long long b = 0;
    size_t size;
    long long i = 0;
    FILE *output;
    long long cont = 0;
    clock_t begin = clock();
    unsigned short off = 0;
    unsigned short lunghezza = 0;
    unsigned char next_char = 0;
    FILE *input;
    long long n = 0;
    input = fopen(filecompresso, "rb");
    output = fopen(fileoutput, "wb");
    if (input == NULL) {
        printf("non esiste il file !!");
        exit(0);
    }
    char *buffer;
    buffer = (char *) malloc(100000 * sizeof(char));
    fseek(input, 0, SEEK_END);
    size = (size_t) ftell(input);
    fseek(input, 0, SEEK_SET);
    unsigned short temp;
    unsigned char buf[19];
    unsigned char tempChar;
    int indice = 19;
    int lettura = 19;
    int bitposition = 0;
    while (i < size) {
        for (int h = 0; h < 19; h++)
            buf[h] = 0;
        lettura = fread(buf, 1, 19, input);   //leggo da file, dovrò controllare quando lettura<19
        for (int k = 0; k < 19;) {

//            printf("\ni=%d k=%d\n",i,k);
            temp = 0;
//            printf("\nbuf: %d\n",buf[k]);
//            printf("\nbitpos :%d\n",bitposition);
            if (bitposition == 0) {
                temp = buf[k] >> 1;
                temp = temp << 1;
            } else {
                temp = buf[k] << bitposition;
                temp = temp << 8;
                temp = temp >> 8;
            }
            bitposition += 7;

            if (bitposition >= 8) {
                k++;
                i++;
                bitposition %= 8;
//                printf("Temp : %d",temp);
                tempChar = (buf[k] >> (8 - (bitposition + 1)));
                temp = temp | tempChar;
            }
            bitposition %= 8;
            temp = temp << 7;
//            printf("Temp spostato di 7: %d",temp);
            tempChar = buf[k] << bitposition;
            temp = temp | (tempChar);
            k++;
            i++;
//            printf("\nbuf: %d\n",buf[k]);
            if (bitposition != 0) {
                tempChar = buf[k] >> (8 - bitposition);
                temp = temp | (tempChar);
            }
//            printf("Temp : %d",temp);
            lunghezza = temp;
            printf("%d", lunghezza);
            //fine maxlength

            temp = 0;
//            printf("\nbuf: %d\n",buf[k]);
//            printf("\nbitpos :%d\n",bitposition);
            if (bitposition == 0) {
                temp = buf[k] >> 1;
                temp = temp << 1;
            } else {
                temp = buf[k] << bitposition;
                temp = temp << 8;
                temp = temp >> 8;
            }
            bitposition += 7;

            if (bitposition >= 8) {
                k++;
                i++;
                bitposition %= 8;
//                printf("Temp : %d",temp);
//                printf("\nbitpos :%d\n",bitposition);
                temp = temp | (buf[k] >> (7 - bitposition));
            }
            bitposition %= 8;
            temp = temp << 7;
//            printf("Temp spostato di 7: %d",temp);
            tempChar = buf[k] << bitposition;
            temp = temp | (tempChar);
            k++;
            i++;
//            printf("\nbuf: %d\n",buf[k]);
            if (bitposition != 0) {
                tempChar = buf[k] >> (8 - bitposition);
                temp = temp | (tempChar);
            }
            //           printf("Temp : %d",temp);
            off = temp;
            printf("%d", off);
//            printf("%d",off);
            //fine foff
            tempChar = 0;
            tempChar = (buf[k] << bitposition);
            k++;
            i++;
//            printf("\nbuf: %d\n",buf[k]);
            if (bitposition != 0)
                tempChar = tempChar | (buf[k] >> (8 - bitposition));
//            printf("Temp : %d",tempChar);
            next_char = tempChar;
            printf("%c\n", next_char);
            //fine char
            printf(" \t indice %d\t", i);
            if (i <= size) {
                for (n = 0; n < lunghezza; n++) {
                    if (cont == 99999 || (i == size)) {

                        if (b != 0) {
                            for (int c = MAX; c < cont; c++) {
                                //printf("%c",buffer[c]);
                                fputc(buffer[c], output);
                            }
                        } else {
                            for (int c = 0; c < cont; c++) {
                                // printf("%c",buffer[c]);
                                fputc(buffer[c], output);
                            }
                            b++;
                        }
                        for (int c = 0; c < MAX; c++) {
                            app[c] = buffer[99999 - MAX + c];
                        }
                        for (int v = 0; v < MAX; v++) {
                            buffer[v] = app[v];
                        }

                        cont = MAX;

                    }
                    buffer[cont] = buffer[cont - off];
                    if (i != size)
                        cont++;

                }
                if (cont == 99999 || (i == size)) {

                    if (b != 0) {
                        for (int c = MAX; c < cont; c++) {
                            //printf("%c",buffer[c]);
                            fputc(buffer[c], output);
                        }
                    } else {
                        for (int c = 0; c < cont; c++) {
                            // printf("%c",buffer[c]);
                            fputc(buffer[c], output);
                        }
                        b++;
                    }
                    for (int c = 0; c < MAX; c++) {
                        app[c] = buffer[99999 - MAX + c];
                    }
                    for (int v = 0; v < MAX; v++) {
                        buffer[v] = app[v];
                    }

                    cont = MAX;
                    buffer[cont] = buffer[cont - off];
                    if (i != size)
                        cont++;
                }
                //              printf("\n%c", buffer[cont]);

                buffer[cont] = next_char;
                if (i != size)
                    cont++;
                if (cont == 99999 || (i == size)) {
                    if (b != 0) {
                        for (int c = MAX; c < cont; c++) {
                            //printf("%c",buffer[c]);
                            fputc(buffer[c], output);
                        }
                    } else {
                        for (int c = 0; c < cont; c++) {
                            // printf("%c",buffer[c]);
                            fputc(buffer[c], output);
                        }
                        b++;
                    }
                    for (int c = 0; c < MAX; c++) {
                        app[c] = buffer[99999 - MAX + c];
                    }
                    for (int v = 0; v < MAX; v++) {
                        buffer[v] = app[v];
                    }

                    cont = MAX;

                }
                //operazioni
            }
        }
        //i--;

    }
    //   fwrite(buffer,1,cont,output);
    fclose(input);
    fclose(output);
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("secondi : %lf", time_spent);
    printf("\nfinish!!!!");
}
int main(int argc,char **args) {
    if(strcmp(args[1],"-c")==0){
        comprimi(args[2],args[3]);//chiamo compri
    }else if(strcmp(args[1],"-d")==0){
        decomprimi(args[2],args[3]);
    }else{
        printf("scelta non valida!!!!!");
    }
}

 */