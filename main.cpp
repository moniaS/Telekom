#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <locale.h>

//#define UNICODE

using namespace std;

const int wiersze = 8; //m w sprawozdaniu
const int bitowNaSlowo = 16;

bool H[wiersze][bitowNaSlowo] = {
                {1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0},
                {0,1,1,1,1,0,1,1,0,1,0,0,0,0,0,0},
                {1,0,0,1,0,1,0,1,0,0,1,0,0,0,0,0},
                {1,1,1,0,1,0,0,1,0,0,0,1,0,0,0,0},
                {0,1,0,1,0,1,1,0,0,0,0,0,1,0,0,0},
                {1,0,1,0,1,0,1,0,0,0,0,0,0,1,0,0},
                {1,1,0,0,1,1,0,0,0,0,0,0,0,0,1,0},
                {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1}};

int Decode[wiersze];       //wektor otrzymany przy dekodowaniu
int ASCII[bitowNaSlowo];
int T[bitowNaSlowo];

void mnozenie(int wektor[], int Wwynikowy[])
{
    for(int i = 0; i < wiersze; i++)
    {
        for(int j = 0; j < bitowNaSlowo; j++)
        {
            Wwynikowy[i] += wektor[j] * H[i][j];
        }
        Wwynikowy[i] %= 2;
    }
}


void AsciiToBin(int znakAscii)
{
	for(int i = 7; i >= 0; i--)
    {
        bool czyParzyste = znakAscii%2;
        T[i] = czyParzyste;
        znakAscii /= 2;
    }
}


void PlAsciiToBin(int znakAscii)
{
    znakAscii += 256;
    AsciiToBin(znakAscii);
}

int *mnozWektorPrzezH(int wektor[])
{
    int* wWynikowy = new int[wiersze];
    for (int i=0; i <wiersze; i++) {
            wWynikowy[i]=0;
        cout<<wWynikowy[i]<<"--<--"<<endl;
    }
    for(int i = 0; i < wiersze; i++)
    {
        for(int j = 0; j < bitowNaSlowo; j++)
        {
            wWynikowy[i] += wektor[j] * H[i][j];
        }
        wWynikowy[i] %= 2;

    }
    return wWynikowy;
}

void kodowanie(char znak)
{
    static int AC[wiersze];
    //przeksztalcenie na kod ASCII
	if(znak > 0) //polskie znaki maja bit znaku ustawiony na 1 wiec sa ujemne
    {
        AsciiToBin(znak);
    }
	else
    {
        PlAsciiToBin(znak);
    }
	//obliczanie sumy kontrolnej na podstawie kodu ASCII
    int* bityParzystosci = new int[wiersze];
    bityParzystosci = mnozWektorPrzezH(T);
    mnozenie(T, AC);
    for (int i=0; i< wiersze; i++) {
        cout<<AC[i]<<"  "<<bityParzystosci[i]<<endl;
    }
    //uzupelnij tablice o bity kontrolne
    for(int i = 8, j = 0; i < bitowNaSlowo; i++, j++)
        T[i] = bityParzystosci[j];
}


int naASCII()
{
    double kodASCII = 0.0;

    for(int i = 0, j = 7; i < 8; i++, j--)
        kodASCII += (ASCII[i] * pow(2.0,j));

    return (int)kodASCII;
}


bool czyBlad()
{
    for(int i = 0; i < wiersze; i++)
        if(Decode[i] != 0) return true;

    return false;
}


int szukaj1Blad() throw(string)
{
    bool found;
    string e = "  Wiecej niz 1 blad, szukam dalej!";


    for(int i = 0; i < bitowNaSlowo; i++)
    {
        found = true;
        for(int j = 0; j < wiersze; j++)
        {
            if(H[j][i] != Decode[j])
            {
                found = false;
                break;
            }
        }
        if(found) return i; //zwraca pozycje, na ktorej znajduje sie blad
    }
    if(!found) throw e;
}


int *szukaj2Bledy() throw(string)
{
    bool found;
    int *k = new int[2];
    string e = "  Wiecej niz 2 bledy!";

    for(int i = 0; i < bitowNaSlowo-1; i++)
    {
        for(int l = i+1; l < bitowNaSlowo; l++)
        {
            found = true;

            for(int j = 0; j < wiersze; j++)
            {
                if(((H[j][i] + H[j][l])%2) != Decode[j])
                {
                    found = false;
                    break;
                }
            }
            if(found)
            {
                k[0] = i;
                k[1] = l;
                return k;
            }
        }
    }
    if(!found) throw e;
}


void naprawBit(int pozycjaBledu)
{
    ASCII[pozycjaBledu] += 1;
    ASCII[pozycjaBledu] %= 2;
}


void napraw2Bledy()
{
    try
    {
        int *pozycjeBledow = new int[2];
        pozycjeBledow = szukaj2Bledy();


        //NAPRAWIANIE
        naprawBit(pozycjeBledow[0]);
        naprawBit(pozycjeBledow[1]);
        cout << "  Bledy na bitach o pozycjach " << pozycjeBledow[0] << " i " << pozycjeBledow[1] << ".";
    }
    catch(string e)
    {
        cout << e;
    }
}


void napraw1Blad()
{
    try
    {
        int pozycjaBledu;
        pozycjaBledu = szukaj1Blad();

        //NAPRAWIANIE
        naprawBit(pozycjaBledu);
        cout << "  Blad na bicie o pozycji " << pozycjaBledu << ".";
    }
    catch(string e)
    {
        napraw2Bledy();
    }
}


char dekodowanie()
{
    for(int i = 0; i < wiersze; i++)
        Decode[i] = 0;
    mnozenie(ASCII, Decode);

    if(czyBlad()) napraw1Blad();
    int kodZnaku = naASCII();
    return char(kodZnaku);
}

string odczytajPlik()
{
    ifstream plik_in;
    string wiadomosc;
    plik_in.open("wysylana.txt");  //otwarcie pliku zawierajacego wiadomosc do "przeslania"
    getline(plik_in,wiadomosc);        //pobranie wiadomosci z pliku
    plik_in.close();
    return wiadomosc;
}

int main()
{
	setlocale(LC_ALL,"polish"); //wyœwietlanie polskich znaków

    ofstream plik_out;
    string wiadomosc = odczytajPlik();

//KODOWANIE

    plik_out.open("odebrana.txt");


    //cout << "Wiadomoœæ do wys³ania: " << wiadomosc << endl;
    cout << endl;

	int i = 0;
    for(string::iterator it = wiadomosc.begin(); it != wiadomosc.end(); ++it)
    {
		if(i >2) //W kodowaniu pliku Windows-1250 musimy pomin¹æ pierwsze 3 znaki.
		{
			kodowanie(*it);
			cout << *it << ": ";
			for(int i = 0; i < bitowNaSlowo; i++)
			{
				plik_out << T[i];
				if(i == bitowNaSlowo-1) plik_out << "\n";
				cout << T[i];
				if(i == 7) cout << " | ";
			}
			cout << endl;
		}
		else i++;
    }

    plik_out.close();
    cout << endl;

    system("PAUSE");
    cout << endl;


//DEKODOWANIE
    char znak;
    string line;
    ifstream plik_in;
    plik_in.open("odebrana.txt");

    cout << "Odebrano: " << endl;
    for(int i = 0; i < wiadomosc.length()-3; i++) //-3 bo w kodowaniu Windows-1250 pierwsze 3 znaki pomijamy
    {
        getline(plik_in,line);
        for(int j = 0; j < bitowNaSlowo; j++)
        {
            ASCII[j] = (int)line[j]-48;
            cout << ASCII[j];
            if(j == 7) cout << " | ";
        }
        for(int l = 0; l < wiersze; l++)
            Decode[l] = 0;
        cout << " : " << dekodowanie() << endl;
    }

    plik_in.close();
    cout << endl;


    system("PAUSE");
    return 0;
}



