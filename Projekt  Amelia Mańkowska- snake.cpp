#include<iostream>
#include<windows.h>
#include<vector>
#include<time.h>
#include<chrono>
#include<conio.h>
#include<ncursesw/curses.h>

using namespace std;
using namespace std::chrono;

//
// Projekt zaliczeniowy PRG - Amelia Mańkowska (s31320)
//

// Ustawienia
const int WIELKOSC = 21;
const int ODSWIEZANIE = 250;

// Zdefiniowanie kierunków
enum Kierunek {
	GORA, 
	DOL, 
	LEWO, 
	PRAWO 
};

// Funkcja losująca wartości od podanej minimalnej do podanej maksymalnej
int losuj(int min, int max) {
	return (rand() + min) % max;
}

// Funkcja rysująca kwadrat na konsoli w danych współrzędnych
void rysujKwadrat(int x, int y, int kolor) {
    x = x * 2;
    y = WIELKOSC - y - 1;
    
    attron(COLOR_PAIR(kolor));
    
    move(y, x);
    addch(' ');
    
    move(y, x + 1);
    addch(' ');
}

// Sprawdza czy nowy kierunek jest przeciwny do starego
// (Aby wąż nie mógł się cofać do tyłu)
bool czyPrzeciwnyKierunek(Kierunek stary, Kierunek nowy) {
	if(stary == GORA && nowy == DOL) return true;
	else if(stary == DOL && nowy == GORA) return true;
	else if(stary == LEWO && nowy == PRAWO) return true;
	else if(stary == PRAWO && nowy == LEWO) return true;
	
	return false;
}

// Obiekt jabłka, które pojawia się na planszy
class Jablko {
	private:
		int x;
		int y;
	
	public:
		Jablko(int x, int y) {
			this -> x = x;
			this -> y = y;
		}
		
		int getX() {
			return this -> x;
		}
		
		int getY() {
			return this -> y;
		}
		
		bool czyWZasiegu(int x, int y) {
			return (getX() == x && getY() == y);
		}
};

// Część ciała węża
class SnakeCzesc {
	private:
		int x;
		int y;
	
	public:
		SnakeCzesc(int x, int y) {
			this -> x = x;
			this -> y = y;
		}
		
		int getX() {
			return this -> x;
		}
		
		int getY() {
			return this -> y;
		}
		
		bool czyWZasiegu(int x, int y) {
			return (getX() == x && getY() == y);
		}
};


// Obiekt węża
class Snake {
	private:
		vector<SnakeCzesc> cialo;
	
	public:
		Snake(int x, int y) {
			this -> cialo.push_back(SnakeCzesc(x, y));
		}
		
		void przesun(int x, int y, bool powieksz) {
			if(!powieksz) cialo.erase(cialo.begin());
			
			this -> cialo.push_back(SnakeCzesc(x, y));
		}
		
		int getX() {
			return this -> cialo.back().getX();
		}
		
		int getY() {
			return this -> cialo.back().getY();
		}
		
		bool czyWZasiegu(int x, int y) {	
			for(int i = 0; i < cialo.size(); i++) {
				if(cialo[i].czyWZasiegu(x, y)) return true;
			}
			
			return false;
		}
		
};

// Obiekt gry
class Gra {
	private:
		Kierunek kierunek = PRAWO;
		Snake snake = stworzSnake();
		Jablko jablko = stworzJablko();
	
	public:
		// Zwraca obiekt węża tej gry (użyłam &, aby zwrócić dokładnie tą wartość)
		Snake& getSnake() {
			return this -> snake;
		}
		
		// Zwraca obiekt jabłka tej gry (użyłam &, aby zwrócić dokładnie tą wartość)
		Jablko& getJablko() {
			return this -> jablko;
		}
		
		// Zwraca obiekt kierunku tej gry (użyłam &, aby zwrócić dokładnie tą wartość)
		Kierunek& getKierunek() {
			return this -> kierunek;
		}
		
		Snake stworzSnake() {
			return Snake(WIELKOSC / 2 + 1, WIELKOSC / 2 + 1);
		}
		
		Jablko stworzJablko() {
			int x;
			int y;
			
			do {
				x = losuj(0, WIELKOSC);
				y = losuj(0, WIELKOSC);
			} while(getSnake().czyWZasiegu(x, y));
			
			
			return Jablko(x,y);
		}
		
		void przesun() {
			int x = getSnake().getX();
			int y = getSnake().getY();
			
			switch(this -> kierunek) {
				case GORA:
					y++;
					break;
				case DOL:
					y--;
					break;
				case PRAWO:
					x++;
					break;
				case LEWO:
					x--;
					break;
			}
			
			x = poprawioneKoordynaty(x);
			y = poprawioneKoordynaty(y);
			
			if(getJablko().czyWZasiegu(x, y)) {
				getSnake().przesun(x, y, true);
				this -> jablko = stworzJablko();
			}
			else if(getSnake().czyWZasiegu(x, y)) { 
				this -> snake = stworzSnake();
			}
			else getSnake().przesun(x, y, false);
		}
		
		int poprawioneKoordynaty(int wspolrzedna) {
			if(wspolrzedna >= WIELKOSC) wspolrzedna = 0;
			else if(wspolrzedna < 0) wspolrzedna = WIELKOSC - 1;
			
			return wspolrzedna;
		}
		
		void setKierunek(Kierunek nowy) {
			Kierunek stary = getKierunek();
			
			if(czyPrzeciwnyKierunek(stary, nowy) == false) {
				this -> kierunek = nowy;
			}
		}
		
		void rysuj() {
			for(int x = 0; x < WIELKOSC; x++) {
				for(int y = 0; y < WIELKOSC; y++) {
					if(getSnake().czyWZasiegu(x, y)) rysujKwadrat(x, y, 2);
					else if(getJablko().getX() == x && getJablko().getY() == y) rysujKwadrat(x, y, 3);
					else rysujKwadrat(x, y, 1);
				}
			}
		}
};

int main() {
	srand(time(NULL));
	Gra gra;
	
	steady_clock::time_point ostatniaKlatka = steady_clock::now(); 
	
	WINDOW * mainwin = initscr();
	keypad(stdscr, TRUE);
	noecho();
	
	start_color();
	
	init_pair( 1, COLOR_WHITE, COLOR_WHITE ); // 1 para kolorów (PLANSZA)
	init_pair( 2, COLOR_GREEN, COLOR_GREEN ); // 2 para kolorów (WĄŻ)
	init_pair( 3, COLOR_RED, COLOR_RED ); // 3 para kolorów (JABŁKO)

	// Pętla gry
    while(true) {
        if(kbhit()) {
            switch (getch()) {
                case KEY_UP:
                    gra.setKierunek(GORA); 
                    break;
                case KEY_DOWN:
                    gra.setKierunek(DOL);
                    break;
                case KEY_LEFT:
                    gra.setKierunek(LEWO);
                    break;
                case KEY_RIGHT:
                    gra.setKierunek(PRAWO);
                    break;
            }
        }
        
    	steady_clock::time_point aktualnaKlatka = steady_clock::now();

        if (duration_cast<milliseconds>(aktualnaKlatka - ostatniaKlatka).count() >= ODSWIEZANIE) {
        	clear();
            gra.przesun();
            
			ostatniaKlatka = aktualnaKlatka;

            gra.rysuj();
            refresh();
        }
    }
}