#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <locale.h>
#include <algorithm>

//#define UNICODE

using namespace std;

const int wiersze = 8; //m w sprawozdaniu
const int bitowNaSlowo = 16; //m+n w sprawozdaniu
const char* PLIK_WYSYLANA = "wysylana.txt";
const char* PLIK_ODEBRANA = "odebrana.txt";
const int PRZESUNIECIE = 0; //W kodowaniu pliku Windows-1250 musimy pominąć pierwsze 3 znaki.

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

bool czyDecodeMaBlad()
{
    for(int i = 0; i < wiersze; i++)
    {
        if(Decode[i] != 0)
            return true;
    }
    return false;
}

//porownujemy z wybrana kolumna macierzy H, szukając bledu
bool czyDecodeRownyKolumnie(int nrKolumny) {
    for (int i = 0; i < wiersze; i++) {
        if (Decode[i] != H[i][nrKolumny])
            return false;
    }
    return true;
}

bool czyDecodeRownySumieKolumn (int nrKolumny1, int nrKolumny2) {
    for (int i = 0; i < wiersze; i++) {
        int sumaKolumn = H[i][nrKolumny1] + H[i][nrKolumny2];
        sumaKolumn %= 2;
        if (Decode[i] != sumaKolumn)
            return false;
    }
    return true;
}

int ktorejKolumnieRownyDecode() {
    for (int wiersz = 0; wiersz < bitowNaSlowo; wiersz++) {
        if (czyDecodeRownyKolumnie(wiersz))
            return wiersz;
    }
    return -1; //jesli zaden nie jest rowny, to zwroc -1
}

int* ktorymDwomKolumnomRownyDecode() {
    for (int i = 0; i < bitowNaSlowo - 1; i++) {
        for (int j = i + 1; j < bitowNaSlowo; j++) {
            if (czyDecodeRownySumieKolumn(i, j)) {
                return new int[2]{i,j};
            }
        }
    }
    return NULL; //jesli zadna para nie jest rowna, to zwroc nulla
}

void naprawBit(int pozycjaBledu)
{
    R[pozycjaBledu] ++;
    R[pozycjaBledu] %= 2;
}

char parsujRnaAscii() {
    int mnoznik = 1;
    int ascii = 0;
    for (int i = wiersze-1; i >= 0; i--) {
        ascii += R[i] * mnoznik;
        mnoznik *= 2;
    }
    return (char) ascii;
}

void naprawZnakIWypiszBledy() {
    int pozycjaBledu = ktorejKolumnieRownyDecode();
    if (pozycjaBledu != -1) {
        naprawBit(pozycjaBledu);
        cout<<" ("<<pozycjaBledu<<") ";
    } else {
        int* pozycjeBledow = ktorymDwomKolumnomRownyDecode();
        if (pozycjeBledow != NULL) {
            naprawBit(pozycjeBledow[0]);
            naprawBit(pozycjeBledow[1]);
            cout<<" ("<<pozycjeBledow[0]<<","<<pozycjeBledow[1]<<") ";
        } else {
            cout<<"3 bledy :((";
        }
    }
}

char dekodujWektorR()
{
    for(int i = 0; i < wiersze; i++)
        Decode[i] = 0;

    mnozenie(R, Decode);

    if(czyDecodeMaBlad())
    {
        naprawZnakIWypiszBledy();
    }
    return parsujRnaAscii();
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
        R[i] = line[i];
        cout << R[i];
        if(i == 7)
            cout << " ";
    }
}

void dekodujPlik() {
    ifstream plik_in;
    plik_in.open(PLIK_ODEBRANA);
    cout << "Odebrano: " << endl;
    string linia;
    while (getline(plik_in, linia))
    {
        parsujIWyswietl(linia);
        cout << " : " << dekodujWektorR() << endl;
    }

    plik_in.close();
    cout << endl;
}

int main()
{
	setlocale(LC_ALL,"polish"); //wyświetlanie polskich znaków
    string wiadomosc = odczytajPlik();
    zakodujIWypisz(wiadomosc);
    system("PAUSE");

    cout << endl;
    dekodujPlik();
    system("PAUSE");

    return 0;
}
