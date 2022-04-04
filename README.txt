Stanciu Bianca-Andreea, 335CC
                                            Tema 3
                                Algoritmi Paraleli si Distribuiti
                            Calcule colaborative in sisteme distribuite

                                    -----------------------------
                                    Etapa1: Stabilirea topologiei
                                    ------------------------------
    Pentru reprezentarea clusterelor am folosit o structura formata din rangul procesului si un vector de workeri.
 Fiecare proces va trebui sa aiba 3 vectori de workeri, unul pentru fiecare proces parinte.

Citire date si trimitere catre procese: functia read_and_send
------------------------------------------------------------------------------------------------------------------------
        Numarul de workeri (= prima linie din fisier) l-am trimis catre fiecare proces, urmand ca urmatoarele valori din
fisier sa fie puse in vectorul de workeri corespunzator procesului. De asemenea workerii vor fi trimisi si catre restul
proceselor pentru ca toate procesele sa cunoasca topologia. Pentru a respecta canalul de comunicatie pentru bonus, in
care procesul 0 si 1 nu au voie sa comunice direct, datele si informatiile despre procesul 0 si 1 vor fi trimise prin
intermediul procesului 2. Astfel 0 va trimite catre doi informatiile sale, iar 2 va comunica aceste informartii
workerilor sai, dar si procesului 1. Analog si procesul 1, va trimite datele sale catre 0 si workerii lui 2 prin
procesul 2.
        De aceea ca parametrii am rank-ul curent si rank-ul catre care va trimite.

Primire informatii: functia recv_data
------------------------------------------------------------------------------------------------------------------------
        Fiecare proces va primi numarul de workeri, pentru a putea sa se constuiasca vectorul de workeri pentru fiecare
proces. Dupa acest lucru, procesele vor cunoaste cei 3 vectori de clustere: clusters[0].workeri, clusters[1].workeri si
clusters[2].workeri. Acesti vectori vor contine workerii corespunzatori.

Trimitere topologie catre workeri: functia send_workers
------------------------------------------------------------------------------------------------------------------------
        Fiecarui worker se va trimite rangul procesului care trimite informatiile, numarul workerilor proceselor 0, 1 si
2 si topologia pentru fiecare proces parinte: vectorul de workeri.
        In workeri se va construi topologia pe baza informatiilor primite: cei trei vectori de dimensiunile corespunza-
toare cu workerii corespunzatori fiecaruia.

Afisare toplogie in fiecare proces: functia printf_topology
------------------------------------------------------------------------------------------------------------------------
       Se va afisa rangul procesului curent, urmand sa se afiseze pentru fiecare proces parinte vectorul de workeri cu
mesajul corespunzator.

                                    -----------------------------
                                    Etapa2: Realizarea calculelor
                                    ------------------------------

        In procesul 0, am initializat vectorul care va trebui prelucrat cu valorile corespunzatoare v[i] = i. Pentru ca
realizarea calculelor sa fie cat mai echilibrata am aflat numarul total de workeri = nr. Numarul de elemente primit ca
argument in cazul in care nu este un multiplu de nr atunci se va calcula un rest, iar elementele ramase conform acestui
rest vor fi prelucrate de ultimul proces. Astfel procesul 0 va trimite N / numar_workeri catre fiecare workeri si va
primi elementul dublat de la fiecare, inlocuindu-ul pe pozitia corespunzatoare in vectorul initial. Procesul 1 va face
acelasi lucru, in timp ce ultimul proces va trebui sa prelucreze numarul de elemente ramase. Procesele 1 si 2 nu vor
trimite direct elementele, ci atunci cand sunt primite de la procesul 0, sunt salvate intr-un vector q, urmand ca acesta
sa fie impartit in N / numar_workeri si trimis catre fiecare workeri corespunzator procesului parinte. Dupa ce fiecare
worker isi face treaba si trimite catre procesul parinte, acest proces parinte va trebui sa trimita catre procesul 0
informatiile prelucrate, acestea fiind puse in vectorul care trebui prelucrat. La final, cand se primesc toate
informatiile, se va afisa rezultatul: elementele vectorului dublate. Pentru a respecta canalul de comunicatie pentru
bonus, procesul 2 va trimite elementele din vector primite de la procesul 0 catre procesul 1 pentru a fi prelucrate de
catre workerii acestuia. Dupa dublarea elementelor, procesul 1 va trimite rezultatul final catre 2, acesta urmand sa il
trimita procesului 0 pentru a fi puse in vector pe pozitiile corespunzatoare.
