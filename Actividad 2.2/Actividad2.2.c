#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 20

typedef struct{
    unsigned char b;
    unsigned char g;
    unsigned char r;
}pixel;

pixel **createMatrix(int width, int height);
pixel **createPadding(pixel **m, int width, int height, int padding);
pixel **copyMatrix(pixel **original, int width, int height);
void convolution(pixel **m, pixel **p, int width, int height, int widthC, int heightC);
void toGrayScale(pixel **m, int width, int height, FILE *file);
void setZeros(pixel **m, int width, int height);
void setCenter(pixel **p, pixel **m,int width, int height, int padding);
void writeImage(pixel **m, int width, int height, FILE *file);
void changeHorizontal(pixel **m, int width, int height);
void blurring(pixel **original, FILE *result, int width, int height, int padding);
void mirrorBlurring(pixel **original, FILE *result, int width, int height, int padding);

int main(int argc, char const *argv[]){
    FILE *image, *imagesResult[N];
    image = fopen("sample2.bmp","rb");
    for(int i = 0; i < N; i++){
        char filename[20];
        sprintf(filename, "%d-image.bmp", i + 1);
        imagesResult[i] = fopen(filename,"wb");
    }

    double t1, t2, tiempo;

    unsigned char *header;
    header = (unsigned char*)malloc(sizeof(unsigned char)*54);

    fread(header, sizeof(unsigned char), 54, image);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];

    fseek(image, 54, SEEK_SET);

    pixel **pixels = createMatrix(width, height);

    for (int i = 0; i < N; i++){
        for (int j = 0; j < 54; j++){
            fputc(header[j], imagesResult[i]);   
        }
    }
    
    toGrayScale(pixels, width, height, image);

    omp_set_num_threads(N);

    const double startTime = omp_get_wtime();
    t1 = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section  //01      
                (void)blurring(pixels, imagesResult[0], width, height, 0);
            #pragma omp section  //02 
                (void)blurring(pixels, imagesResult[1], width, height, 4);           
            #pragma omp section  //03
                (void)blurring(pixels, imagesResult[2], width, height, 8);
            #pragma omp section  //04
                (void)blurring(pixels, imagesResult[3], width, height, 12);
            #pragma omp section  //05
                (void)blurring(pixels, imagesResult[4], width, height, 16);
            #pragma omp section  //06
                (void)blurring(pixels, imagesResult[5], width, height, 20);
            #pragma omp section  //07
                (void)blurring(pixels, imagesResult[6], width, height, 24);
            #pragma omp section  //08
                (void)blurring(pixels, imagesResult[7], width, height, 28);
            #pragma omp section  //09
                (void)blurring(pixels, imagesResult[8], width, height, 32);
            #pragma omp section  //10
                (void)blurring(pixels, imagesResult[9], width, height, 36);
            #pragma omp section  //11     
                (void)mirrorBlurring(pixels, imagesResult[10], width, height, 0);               
            #pragma omp section  //12 
                (void)mirrorBlurring(pixels, imagesResult[11], width, height, 4);
            #pragma omp section  //13
                (void)mirrorBlurring(pixels, imagesResult[12], width, height, 8);
            #pragma omp section  //14
                (void)mirrorBlurring(pixels, imagesResult[13], width, height, 12);
            #pragma omp section  //15
                (void)mirrorBlurring(pixels, imagesResult[14], width, height, 16);
            #pragma omp section  //16
                (void)mirrorBlurring(pixels, imagesResult[15], width, height, 20);
            #pragma omp section  //17
                (void)mirrorBlurring(pixels, imagesResult[16], width, height, 24);
            #pragma omp section  //18
                (void)mirrorBlurring(pixels, imagesResult[17], width, height, 28);
            #pragma omp section  //19
                (void)mirrorBlurring(pixels, imagesResult[18], width, height, 32);
            #pragma omp section  //20
                (void)mirrorBlurring(pixels, imagesResult[19], width, height, 36);
        }
    }

    const double endTime = omp_get_wtime();

    t2 = omp_get_wtime();
    tiempo = t2 - t1;

    printf("tomo (%lf) segundos\n", tiempo);

    free(pixels);
    free(header);

    fclose(image);
    for (int i = 0; i < N; i++){
        fclose(imagesResult[i]);
    }
    return 0;
}

pixel **createPadding(pixel **m, int width, int height, int padding){
    pixel **p = createMatrix(width, height);
    setZeros(p, width, height);
    setCenter(p, m, width, height, padding);

    return p;
}

pixel **createMatrix(int width, int height){
    pixel **m = (pixel**)malloc(sizeof(pixel*)*height);
    
    if(m == NULL){
        printf("Falling malloc\n");
        exit(-1);
    }

    for (int i = 0; i < height; i++){
        m[i] = (pixel*)malloc(sizeof(pixel)*width);
        if(m[i] == NULL){
            printf("Falling malloc");
            exit(-1);
        }
    }

    return m;
}

void setZeros(pixel **m, int width, int height){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            m[i][j].b = 0;
            m[i][j].g = 0;
            m[i][j].r = 0;      
        }
    }
}

void setCenter(pixel **p, pixel **m,int width, int height, int padding){
    for (int i = padding/2; i < height - padding/2; i++){
        for (int j = padding/2; j < width - padding/2; j++){
            p[i][j] = m[i - padding/2][j - padding/2];
        }
    }
}

void toGrayScale(pixel **m, int width, int height, FILE *file){
    unsigned char r, g, b;

    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            b = fgetc(file);
            g = fgetc(file);
            r = fgetc(file);

            unsigned char p = 0.21*r+0.72*g+0.07*b;

            m[i][j].b = p;
            m[i][j].g = p;
            m[i][j].r = p;           
        }
    }
}

void writeImage(pixel **m, int width, int height, FILE *file){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            fputc(m[i][j].b, file);
            fputc(m[i][j].g, file);
            fputc(m[i][j].r, file);
        }
    }
}

void changeHorizontal(pixel **m, int width, int height){
    pixel * array;
    array = (pixel*)malloc(sizeof(pixel)*width);

    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            array[j] = m[i][j];
        }

        for (int j = 0; j < width; j++){
            m[i][j] = array[width - j - 1];
        }   
    }

    free(array);
}

void convolution(pixel **m, pixel **p, int width, int height, int widthC, int heightC){

    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            unsigned int value = 0;
            for (int x = 0; x < heightC; x++){
                for (int y = 0; y < widthC; y++){
                    value += p[i + x][j + y].b;             
                }
            }
            m[i][j].b = value/(heightC*widthC);
            m[i][j].g = value/(heightC*widthC);
            m[i][j].r = value/(heightC*widthC);
        }
    }
}

pixel **copyMatrix(pixel **original, int width, int height){
    pixel **copy = createMatrix(width, height);

    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            copy[i][j] = original[i][j];   
        }
    }

    return copy;  
}

void blurring(pixel **original, FILE *result, int width, int height, int padding){
    if(padding > 0){
        pixel **m = copyMatrix(original, width, height);
        int widthP = width + padding;
        int heightP = height + padding;

        int widthC = padding + 1;
        int heightC = padding + 1;


        pixel **paddedMatrix = createPadding(m, widthP, heightP, padding);
        convolution(m, paddedMatrix, width, height, widthC, heightC);
        
        free(paddedMatrix);
        
        writeImage(m, width, height, result);
        free(m);
    }
    else{
        pixel **m = copyMatrix(original, width, height);
        writeImage(m, width, height, result);  
        free(m);     
    }
}

void mirrorBlurring(pixel **original, FILE *result, int width, int height, int padding){

    if(padding > 0){
        pixel **m = copyMatrix(original, width, height);

        int widthP = width + padding;
        int heightP = height + padding;

        int widthC = padding + 1;
        int heightC = padding + 1;

        changeHorizontal(m, width, height);

        pixel **paddedMatrix = createPadding(m, widthP, heightP, padding);
        convolution(m, paddedMatrix, width, height, widthC, heightC);
        
        writeImage(m, width, height, result);
        
        free(paddedMatrix);
        free(m);
    }
    else{
        pixel **m = copyMatrix(original, width, height);
        changeHorizontal(m, width, height);
        writeImage(m, width, height, result);
        free(m);
    }    
}