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
const int bitowNaSlowo = 16; //m+n w sprawozdaniu
const char* PLIK_WYSYLANA = "wysylana.txt";
const char* PLIK_ODEBRANA = "odebrana.txt";
const int PRZESUNIECIE = 3; //W kodowaniu pliku Windows-1250 musimy pominąć pierwsze 3 znaki.

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
int R[bitowNaSlowo];
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
int* mnozWektorPrzezH(int wektor[])
{
    int* wWynikowy = new int[wiersze];
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

void kodowanie(char znak)
{
	if(znak > 0) //polskie znaki maja bit znaku ustawiony na 1 wiec sa ujemne
    {
        AsciiToBin(znak);
    }
	else
    {
        PlAsciiToBin(znak);
    }
	//obliczanie sumy kontrolnej na podstawie kodu ASCII
	static int bityKontrolne[wiersze];
    mnozenie(T,bityKontrolne);

    //uzupelnij tablice o bity kontrolne
    for(int i = 0; i < wiersze; i++)
    {
        T[i + wiersze] = bityKontrolne[i];
    }
}


int naASCII()
{
    double kodASCII = 0.0;

    for(int i = 0, j = 7; i < 8; i++, j--)
        kodASCII += (R[i] * pow(2.0,j));

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
    R[pozycjaBledu] += 1;
    R[pozycjaBledu] %= 2;
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
    mnozenie(R, Decode);

    if(czyBlad()) napraw1Blad();
    int kodZnaku = naASCII();
    return char(kodZnaku);
}

string odczytajPlik()
{
    ifstream plik_in;
    string wiadomosc;
    plik_in.open(PLIK_WYSYLANA);  //otwarcie pliku zawierajacego wiadomosc do "przeslania"
    getline(plik_in,wiadomosc);        //pobranie wiadomosci z pliku
    plik_in.close();
    return wiadomosc;
}

//Pobiera bity z tablicy T w ktorej jest akrualnie wysylane slowo
void wypiszBityDoPlikuIkonsoli(ofstream &plik_out) {
    for(int i = 0; i < bitowNaSlowo; i++)
    {
        plik_out << T[i];
        cout << T[i];
        if(i==7)
            cout<<" ";
    }
    plik_out << "\n";
    cout << endl;
}

void zakodujIWypisz(string wiadomosc) {
    ofstream plik_out;
    plik_out.open(PLIK_ODEBRANA);
    for(string::iterator it = wiadomosc.begin() + PRZESUNIECIE; it != wiadomosc.end(); ++it)
    {
        kodowanie(*it);
        cout << *it << ": ";
        wypiszBityDoPlikuIkonsoli(plik_out);
    }
    plik_out.close();
}

void parsujIWyswietl(string line) {
    for(int i = 0; i <bitowNaSlowo; i++)
    {
        R[i] = line[i]-'0';
        cout << R[i];
        if(i == 7)
            cout << " ";
    }
}

int main()
{
	setlocale(LC_ALL,"polish"); //wyświetlanie polskich znaków
    string wiadomosc = odczytajPlik();
    zakodujIWypisz(wiadomosc);

    system("PAUSE");
    cout << endl;

    char znak;
    ifstream plik_in;
    plik_in.open("odebrana.txt");

    cout << "Odebrano: " << endl;
    for(int i = 0; i < wiadomosc.length()-PRZESUNIECIE; i++)
    {
        string line;
        getline(plik_in,line);
        parsujIWyswietl(line);

        for(int l = 0; l < wiersze; l++)
            Decode[l] = 0;
        cout << " : " << dekodowanie() << endl;
    }

    plik_in.close();
    cout << endl;


    system("PAUSE");
    return 0;
}






