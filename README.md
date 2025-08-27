# Diplomski_analiza_dionica
Ovo je projekt koji je bio napravljen za praktični dio diplomskog rada "Metode komuniciranja komponenti za heterogene računalne platforme".

Cilj programa je prikazati komunikaciju između CPU-a i GPU-a na praktičnom primjeru koristeći programski jezik C i programski okvir OpenCL.

Projekt je izrađen koristeći Microsoft Visual Studio i CMake.
Da bi projekt radio je također potrebno imati skinuti OpenCL SDK koji je dostupan [ovdje](https://github.com/KhronosGroup/OpenCL-SDK), korištena je verzija v2025.07.23. Može se buildati ili skinuti već buildanu verziju.

Dataset vrijednosti dionica koje sam koristio sa ovim projektom je javno dostupan na [ovoj](https://www.kaggle.com/datasets/jacksoncrow/stock-market-dataset) stranici.

Za operacije vezane za GUI i podatkovni sustav sam koristio Windows API, pa radi samo na Windows operacijskim sustavima.

Datoteke zaglavlja sadrže razne opcije, jedina interesantna je opcija maksimalnog broja datoteka dionica za pročitati (CSVreading.h), default je 10, za velike brojeve datoteka treba puno vremena da se učitaju.
