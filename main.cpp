#include<iostream>
#include<vector>
#include<cmath>
#include<limits>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include<cstdio>
#include<cstring>

#define LUT_SIZE 17
#define IMPORT_LUT_SIZE 32

#define DEBUG

using namespace std;

double **importArray;
int lut_size = 33;

int file_import(const char *fileName)
{
    ifstream ifile(fileName);
    if(!ifile)
    {
        cout << "File open error! " << endl;
        return -1;
    }
    int size = 0;
    int lutType = 0;
    int maxVal = 0;

    string extention(fileName);
    istringstream extenstream(extention);
    string tmpBuff;

    while(getline(extenstream,tmpBuff,'.')) ;

    cout << "extention : " << tmpBuff << endl;

    string sbuffer("START");
    if(tmpBuff.compare("CUBE") == 0)
    {
        while(sbuffer != "#LUT size")
            getline(ifile,sbuffer);

        getline(ifile,sbuffer);

        istringstream streamString(sbuffer);
        string stok;
    
        while(getline(streamString,stok,' '))   ;

        size = stoi(stok);
        cout << "LUT SIZE : " << size << endl;

        while(sbuffer != "#LUT data points")
            getline(ifile,sbuffer);
        lutType = 0;
    }
    else if(tmpBuff.compare("cube") == 0)
    {
        getline(ifile,sbuffer);
        
        cout << "sbuffer : " << sbuffer << endl;
        
        istringstream streamString(sbuffer);
        string stok;

        while(getline(streamString,stok,' '))   ;
        size = stoi(stok);
        cout << "LUT SIZE : " << size << endl;
        lutType = 1;
    }
    else if(tmpBuff.compare("3dl") == 0)
    {
        while(getline(ifile,sbuffer))
        {
            if(sbuffer[0] != '#')
            {
                cout << sbuffer << endl;
                break;
            }
        }
        istringstream streamString(sbuffer);
        string stok;
        vector<int> sizeVec;
        while(getline(streamString,stok,' '))
            sizeVec.push_back(stoi(stok));
        size = sizeVec.size();
        maxVal = sizeVec.back();
        cout << " 3dl size : " <<  size << " maxval : " << maxVal << endl;
        for(int i = 0; i < size; i++)
            cout << sizeVec[i] << endl;
        lutType = 2;
    }
    else
    {
        cout << "File Format Error! format : " << tmpBuff << endl;
        return 0;
    }

    importArray = new double*[size*size*size];
    for(int i = 0; i<size*size*size; i++)
    {
        importArray[i] = new double[3];
    }
    int scount = 0;
    while(getline(ifile,sbuffer))
    {
        istringstream streamStr(sbuffer);
        string stok2;
        for(int i = 0; i < 3; i++)
        {
            getline(streamStr,stok2,' ');
            if(lutType == 2)
                importArray[scount][i] = (double)stod(stok2)/maxVal;
            else
                importArray[scount][i] = stod(stok2);
        }
        scount++;
    }

    for(int i = 0; i < 76; i++)
    {
        printf("%1.5f %1.5f %1.5f\n",importArray[i][0],importArray[i][1],importArray[i][2]);
    }
    ifile.close();
    return size;
}

int main(void)
{
    double inputArray[][LUT_SIZE*LUT_SIZE*LUT_SIZE][3] =
    {
        #include "oriGamutTable.h"
    };
    
    lut_size = file_import("Paladin 1875.CUBE");
    if(lut_size <= 0)
    {
        cout << "Size error!" << endl;
        return -1;
    }

    vector<double> ary1;
    vector<double> ary2;
    for(int i = 1; i<=LUT_SIZE; i++)
    {
        ary1.push_back((double)i/LUT_SIZE);
    }
    for(int i = 1; i<=lut_size; i++)
    {
        ary2.push_back((double)i/lut_size);  
    }
    
    double calculateVal[LUT_SIZE][2] = {0.0f,};

    vector<pair<int,int>> index;
    for(int i = 0; i<LUT_SIZE; i++)
    {
        for(int j = 0; j<lut_size; j++)
        {
            // val same
            if(fabs(ary2[j] - ary1[i]) < numeric_limits<double>::epsilon())
            {
                index.push_back(make_pair(j,j));
                break;
            }
			else if(ary2[j] > ary1[i])
            {
                index.push_back(make_pair(j-1,j));
                break;
            }
        }
    }

    for(int i = 0; i<LUT_SIZE; i++)
    {
        for(int j = 0; j<lut_size; j++)
        {
            if(fabs(ary2[j] - ary1[i]) < numeric_limits<double>::epsilon())
            {
                calculateVal[i][0] = 1;
                calculateVal[i][1] = 0;
                break;
            }
            
			else if(ary2[j] > ary1[i])
            {
                double x = (ary1[i] - ary2[j-1]) / (ary2[j] - ary2[j-1]);
                calculateVal[i][0] = 1-x;
                calculateVal[i][1] = x;
                break;
            }

        }
    }

#ifdef DEBUG
    //debug
    for(int i = 0; i<LUT_SIZE; i++)
        printf("%8d  ",i);
    cout << endl;
    for(int i = 0; i<LUT_SIZE; i++)
        printf("%1.6f  ",ary1[i]);
    cout << endl;

    for(int i = 0; i<lut_size; i++)
    {
        printf("%2d %1.6f\n",i,ary2[i]);
    }
    cout << "Pair" << endl;
    for(int i = 0; i<LUT_SIZE; i++)
    {
        printf("%2d  %2d %2d\n",i,index[i].first,index[i].second);
    }
    cout << "Input LUT" << endl;
    for(int i = 0; i<LUT_SIZE; i++)
    {
        printf("%2d  %1.6f\n",i,inputArray[0][i][0]);
    }
    cout << "Custom LUT" << endl;
    for(int i = 0; i < lut_size; i++)
    {
        printf("%2d  %1.6f\n",i,importArray[i][2]);
    }
    cout << "calculval LUT" << endl;
    for(int i = 0; i<LUT_SIZE; i++)
    {
        printf("%2d  %1.6f %1.6f\n",i,calculateVal[i][0],calculateVal[i][1]);
    }
    // a  16 0 1    b  31 0 1
    // a  13 4 1    b  31 
#endif

    double percent[LUT_SIZE*LUT_SIZE*LUT_SIZE][3] = {0.0f,};
    double resArray[LUT_SIZE*LUT_SIZE*LUT_SIZE][3] = {0.0f,};
    double resBypassArray[LUT_SIZE*LUT_SIZE*LUT_SIZE][3] = {0.0f,};    
    unsigned short shortArray[LUT_SIZE*LUT_SIZE*LUT_SIZE * 3] = {0,};
    unsigned short shortBypassArray[LUT_SIZE*LUT_SIZE*LUT_SIZE * 3] = {0,};
    int maxVal = 4095;
    double x = 0.0f;

    double bypassArray[][LUT_SIZE*LUT_SIZE*LUT_SIZE][3] =
    {
        #include "bypassTable.h"
    };

    FILE* pp;
    FILE* fp;
    FILE* bp;
    pp = fopen("test.txt","w+");
    fp = fopen("result.txt","w+");
    bp = fopen("bypass.txt","w+");
    resArray[0][0] = inputArray[0][0][0] * importArray[0][0];
    resArray[0][1] = inputArray[0][0][1] * importArray[0][1];
    resArray[0][2] = inputArray[0][0][2] * importArray[0][2];
    shortArray[0] = (unsigned short)((resArray[0][0] * 4095) + 0.5);
    shortArray[1] = (unsigned short)((resArray[0][1] * 4095) + 0.5);
    shortArray[2] = (unsigned short)((resArray[0][2] * 4095) + 0.5);
    shortBypassArray[0] = 0;
    shortBypassArray[1] = 0;
    shortBypassArray[2] = 0;
    resArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE)-1][0] = inputArray[0][(LUT_SIZE*LUT_SIZE*LUT_SIZE)-1][0] * importArray[(lut_size*lut_size*lut_size)-1][0];
    resArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE)-1][1] = inputArray[0][(LUT_SIZE*LUT_SIZE*LUT_SIZE)-1][1] * importArray[(lut_size*lut_size*lut_size)-1][1];
    resArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE)-1][2] = inputArray[0][(LUT_SIZE*LUT_SIZE*LUT_SIZE)-1][2] * importArray[(lut_size*lut_size*lut_size)-1][2];
    shortArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-3] = (unsigned short)(resArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE)-1][0]*4095+0.5);
    shortArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-2] = (unsigned short)(resArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE)-1][1]*4095+0.5);
    shortArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-1] = (unsigned short)(resArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE)-1][2]*4095+0.5);
    shortBypassArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-3] = (unsigned short)((importArray[(lut_size*lut_size*lut_size)-1][0] * 4095) + 0.5);
    shortBypassArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-2] = (unsigned short)((importArray[(lut_size*lut_size*lut_size)-1][1] * 4095) + 0.5);
    shortBypassArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-1] = (unsigned short)((importArray[(lut_size*lut_size*lut_size)-1][2] * 4095) + 0.5);



    fprintf(fp,"%5d  %2d %2d %2d %1.6f %1.6f %1.6f %5d %5d %5d\n",0,0,0,0,resArray[0][0],resArray[0][1],resArray[0][2],(unsigned short)(resArray[0][0]*4095+0.5),(unsigned short)(resArray[0][1]*4095+0.5),(unsigned short)(resArray[0][2]*4095+0.5));
    fprintf(pp,"%5d,%5d,%5d,\n",shortArray[0],shortArray[1],shortArray[2]);
    fprintf(bp,"%5d,%5d,%5d,\n",shortBypassArray[0],shortBypassArray[1],shortBypassArray[2]);
    for(int a = 1; a < ((LUT_SIZE*LUT_SIZE*LUT_SIZE) - 1 ); a++)
    {
        int red = a % LUT_SIZE;
        int green = ( a / LUT_SIZE ) % LUT_SIZE;
        int blue = ( a / (LUT_SIZE * LUT_SIZE) ) % LUT_SIZE;
        int offset = 0;
        
        int convRed[2] = { index[red].first, index[red].second };
        int convGreen[2] = { index[green].first, index[green].second };
        int convBlue[2] = { index[blue].first, index[blue].second };

        double rBase[4][3] = {0.0f,};
        double gBase[2][3] = {0.0f,};
        double bBase[3] = {0.0f,};

        // https://community.acescentral.com/uploads/default/original/2X/5/518c5ede1ca11c4a7e13f9c7350e2228bb8762c7.pdf
        // Use Trilinear interpolation

        //red
        // 0, 0
        offset = (convGreen[0] * lut_size) + (convBlue[0] * lut_size * lut_size);
        for(int i = 0; i<3; i++)
            rBase[0][i] = importArray[convRed[0] + offset][i] * calculateVal[red][0] + importArray[convRed[1] + offset][i] * calculateVal[red][1];

        // 0, 1
        offset = (convGreen[0] * lut_size) + (convBlue[1] * lut_size * lut_size);
        for(int i = 0; i<3; i++)
            rBase[1][i] = importArray[convRed[0] + offset][i] * calculateVal[red][0] + importArray[convRed[1] + offset][i] * calculateVal[red][1];

        // 1, 0
        offset = (convGreen[1] * lut_size) + (convBlue[0] * lut_size * lut_size);
        for(int i = 0; i<3; i++)
            rBase[2][i] = importArray[convRed[0] + offset][i] * calculateVal[red][0] + importArray[convRed[1] + offset][i] * calculateVal[red][1];

        // 1, 1
        offset = (convGreen[1] * lut_size) + (convBlue[1] * lut_size * lut_size);
        for(int i = 0; i<3; i++)
            rBase[3][i] = importArray[convRed[0] + offset][i] * calculateVal[red][0] + importArray[convRed[1] + offset][i] * calculateVal[red][1];


        //green
        // 0 0 + 1 0
        for(int i = 0; i<3; i++)
            gBase[0][i] = rBase[0][i] * calculateVal[green][0] + rBase[2][i] * calculateVal[green][1];
        // 0 1 +  1 1
        for(int i = 0; i<3; i++)
            gBase[1][i] = rBase[1][i] * calculateVal[green][0] + rBase[3][i] * calculateVal[green][1];


        //blue 0 1
        for(int i = 0; i<3; i++)
            bBase[i] = gBase[0][i] * calculateVal[blue][0] + gBase[1][i] * calculateVal[blue][1];

        double tmpDouble[3] = {0.0f,};
        
        tmpDouble[0] = bBase[0] / ary1[red];
        tmpDouble[1] = bBase[1] / ary1[green];
        tmpDouble[2] = bBase[2] / ary1[blue];

        for(int i = 0; i < 3; i++)
        {
            // 0 case
            if(fabs(inputArray[0][a][i]) < numeric_limits<double>::epsilon())
            {
                if(red == 0)
                {
                    resArray[a][i] = tmpDouble[i] * ary1[red];
                    resBypassArray[a][i] = bBase[i] * ary1[red];
                }
                if(green == 0)
                {
                    resArray[a][i] = tmpDouble[i] * ary1[green];
                    resBypassArray[a][i] = bBase[i] * ary1[green];
                }
                if(blue == 0)
                {
                    resArray[a][i] = tmpDouble[i] * ary1[blue];
                    resBypassArray[a][i] = bBase[i] * ary1[blue];
                }
            }
            else
            {
                resArray[a][i] = (tmpDouble[i] * inputArray[0][a][i]) > 1.0f ? 1.0f : (tmpDouble[i] * inputArray[0][a][i]);
                resBypassArray[a][i] = bBase[i];
            }
        }

        shortArray[a*3] = (unsigned short)((resArray[a][0] * 4095) + 0.5);
        shortArray[(a*3)+1] = (unsigned short)((resArray[a][1] * 4095) + 0.5);
        shortArray[(a*3)+2] = (unsigned short)((resArray[a][2] * 4095) + 0.5);
        shortBypassArray[a*3] = (unsigned short)((resBypassArray[a][0] * 4095) + 0.5);
        shortBypassArray[(a*3)+1] = (unsigned short)((resBypassArray[a][1] * 4095) + 0.5);
        shortBypassArray[(a*3)+2] = (unsigned short)((resBypassArray[a][2] * 4095) + 0.5);
        fprintf(fp,"%5d  %2d %2d %2d %1.6f %1.6f %1.6f %5d %5d %5d\n",a,red,green,blue,resArray[a][0],resArray[a][1],resArray[a][2],(unsigned short)(resArray[a][0]*4095+0.5),(unsigned short)(resArray[a][1]*4095+0.5),(unsigned short)(resArray[a][2]*4095+0.5));
        fprintf(pp,"%5d,%5d,%5d,\n",shortArray[a*3],shortArray[(a*3)+1],shortArray[(a*3)+2]);
        fprintf(bp,"%5d,%5d,%5d,\n",shortBypassArray[a*3],shortBypassArray[(a*3)+1],shortBypassArray[(a*3)+2]);
    }
    
    fprintf(fp,"%5d  %2d %2d %2d %1.6f %1.6f %1.6f %5d %5d %5d\n",LUT_SIZE*LUT_SIZE*LUT_SIZE-1,16,16,16,resArray[LUT_SIZE*LUT_SIZE*LUT_SIZE-1][0],resArray[LUT_SIZE*LUT_SIZE*LUT_SIZE-1][1],resArray[LUT_SIZE*LUT_SIZE*LUT_SIZE-1][2],(unsigned short)(resArray[LUT_SIZE*LUT_SIZE*LUT_SIZE-1][0]*4095+0.5),(unsigned short)(resArray[LUT_SIZE*LUT_SIZE*LUT_SIZE-1][1]*4095+0.5),(unsigned short)(resArray[LUT_SIZE*LUT_SIZE*LUT_SIZE-1][2]*4095+0.5));
    fprintf(pp,"%5d,%5d,%5d,\n",shortArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-3],shortArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-2],shortArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-1]);
    fprintf(bp,"%5d,%5d,%5d,\n",shortBypassArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-3],shortBypassArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-2],shortBypassArray[(LUT_SIZE*LUT_SIZE*LUT_SIZE*3)-1]);
    fclose(bp);
    fclose(fp);
    fclose(pp);

    for(int i =0; i<lut_size*lut_size*lut_size; i++)
        delete importArray[i];
    delete importArray;

    return 0;
}