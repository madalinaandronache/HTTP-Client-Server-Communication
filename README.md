# Protocoale de comunicatii - Tema 4

Student: Andronache Madalina-Georgiana

Grupa: 322CC

Urmatorul fisier contine informatii despre rezolvarea cerintelor propuse in 
tema 4 de la PCom: Client web. Comunicatie cu REST API.

Cea mai mare provocare intalnita a fost rezolvarea corecta si cat mai eficienta
a problemei propuse intr-un timp cat mai scurt. Aceasta tema a fost rezolvata
pe parcursul a 2 zile: in total am lucrat la aceasta tema aproximativ 15 h. 
Punctajul obtinut la testarea locala este de 100/100 pct (treceau toate testele
de pe checker si din sesiuni generate de mine).

## Cuprins:
1. Schelet
2. Requests
3. Client

---
Implementarea functionalitatilor descrise a fost realizata intr-un fisier 
principal: `client.cpp`. In plus, avem fisierele secundare helpers.cpp, requests.cpp
si header-ele `helpers.hpp` si `requests.hpp` care contine definirea functiilor.
si a constantelor utilizate mai apoi in `client.cpp`:

```
// Spre exemplu rutele

#define REGISTER_ROUTE "/api/v1/tema/auth/register"
#define LOGIN_ROUTE "/api/v1/tema/auth/login"
#define LIBRARY_ACCESS_ROUTE "/api/v1/tema/library/access"
#define LOGOUT_ROUTE "/api/v1/tema/auth/logout"
#define LIBRARY_BOOKS_ROUTE "/api/v1/tema/library/books"
```

# 1. Schelet

Am pornit de la scheletul din laboratorul 9, asa cum este mentionat si in 
resursele temei. Am ales sa folosesc **C++**, datorita simplitatii lucrului cu
string-uri, in loc de a folosi *char, astfel, am modificat functiile astfel
incat sa aiba ca valoare returnata string-uri si sa proceseze string-uri
(parametrii sa fie tot string-uri).
In plus, pe langa functiile pentru **POST** si **GET**, din `request.cpp`,
am adaugat si functia pentru **DELETE**:

```
std::string compute_delete_request(string host, string url, string query_params,
							string cookies[], int cookies_count, string token);
```

# 2. Requests

Functiile care implementeaza HTTP Request sunt implementate in fisierul 
`request.cpp`:

Conform informatiilor din laborator:

```
    GET /search/top/?q=programare%20%web%202020 HTTPS\r\n
    Host: facebook.com\r\n
    User-Agent: Mozilla/5.0\r\n
    Connection: keep-alive\r\n
    Cookie: c_user=XXXXXXXXXX; presence=XXXXXXX\r\n
    \r\n
```

Cele 3 functii sunt similare ca si implementare, intrucat header-ul request-ului 
va contine: TIPUL cererii, HOST-ul, TOKEN & COOKIES (aici ma refer la LOGIN 
COOKIE) si un '\r\n' la final.

# 3. Client

Clientul este un program scris in C++ pentru a accepta comenzi de la tastatura 
si trimite, in functie de comanda, cereri catre server. Scopul lui este de a 
functiona ca o interfata in linia de comanda (CLI) cu biblioteca virtuala.

## 3.1. Parsare JSON

Pentru a parsa raspunsurile primite de la server am folosit biblioteca C++
`nlohmann`. 

## 3.2. Comenzile primite de client

* register - efectueaza inregistrarea. Procesul este urmatorul:
Se citesc datele de inregistare de la consola, se creaza un obiect JSON cu
aceste credentiale, se trimite acest obiect la server prin intermediul unei
cereri POST. Se proceseaza raspunsul de la server si se afiseaza un mesaj 
corespuzator cu statusul (***ERROR/SUCCESS***).

```
void register_user(int sockfd);
```

* login - efectueaza autentificarea. Procesul este urmatorul:
Se citesc datele de autentificare de la consola, se creaza un obiect JSON cu
aceste credentiale, se trimite acest obiect la server prin intermediul unei
cereri POST. Se proceseaza raspunsul de la server si se afiseaza un mesaj 
corespuzator cu statusul (***ERROR/SUCCESS***). In caz de reusita, se extrage 
***cookie-ul de login*** care este salvat pentru a fi folosit in cererile urmatoare,
demonstrand faptul ca utilizatorul este autentificat.

```
string login_user(int sockfd);
```

* enter_library - cere acces in biblioteca. Procesul este urmatorul:
Se trimite catre server o cerere GET, incluzand ***cookie-ul de login***.
Se proceseaza raspunsul de la server si se afiseaza un mesaj 
corespuzator cu statusul (***ERROR/SUCCESS***). In caz de reusita, se extrage 
un ***token JWT(JSON Web Token)***  care este salvat pentru a fi folosit 
in cererile urmatoare, demonstrand faptul ca utilizatorul are acces la 
biblioteca.

```
string enter_library(int sockfd, string cookie);
```
* comenzi de procesare a datelor despre carti. 
    - get_books - cere toate cartile de pe server. Trimite o cerere GET 
    pentru a lista cartile, folosind tokenul JWT pentru autorizare.
    - get_book - cere informatie despre o carte. 
    - add_book - adauga o carte. Trimite o cerere GET. 
    cu detaliile cartii sub forma de JSON, inclusiv tokenul JWT pentru 
    a adauga o carte noua.
    - delete_book - sterge o carte. Trimite o cerere DELETE pentru a 
    elimina o carte, specificand ID-ul cartii si folosind tokenul JWT 
    pentru autorizare.

```
void get_books(int sockfd, string cookie, string token);
void get_book(int sockfd, string cookie, string token);
void add_book(int sockfd, string cookie, string token);
void delete_book(int sockfd, string cookie, string token);
```

* logout - efectueaza delogarea utilizatorului. Trimite o cerere GET 
pentru a anunta serverul ca sesiunea curenta trebuie incheiata. Sterge cookie-ul 
de sesiune si tokenul JWT din starea locala a clientului, 
resetand starea de autentificare.

```
void logout_user(int sockfd, string &cookie, string &token);
```

* exit - efectueaza iesirea din program. La trimiterea acestei comenzi, 
programul isi incheie executia.

## 3.3. Functii auxiliare

```
// Extrage codul de status HTTP din raspunsul primit de la server
int get_status_code(string response);

// Extrage mesajul de eroare din raspunsul primit de la server
json get_error(string response);

// Returneaza cookie-ul de login din raspunsul primit de server
string get_cookie(string response);

// Returneaza token-ul din raspunsul primit de la server
string get_token(string response);

// Afiseaza detalille cerute despre carti din raspunsul serverului
void list_books(string response);

// Afiseaza detaliile unei carti specifice din raspunsul serverului
void details_book(string response);
```

