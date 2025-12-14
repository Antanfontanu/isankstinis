# OOP išankstinė egzamino užduotis

# Programos aprašymas

Ši programa yra skirta analizuoti tekste esančių žodžių dažnumą, poziciją tekste (kurioje eilutėje tas tikras žodis yra) ir nuorodų radimą.

# Paleidimo instrukcijos

### Reikalavimai
- C++17 suderinamas kompiliatorius
- MinGW / Git Bash arba PowerShell Windows aplinkoje

## Naudojant CMake
1. Nukopijuokite projekto direktoriją į savo kompiuterį bei sukurkite build direktoriją <br>
    ```
   cd projekto_direktorija
   mkdir build
   cd build
   ```
2. Su Cmake generuojame reikalingus projekto failus, nurodykite kelią iki CmakeLists.txt failo <br>
   ```
   cmake ..
   ```
3. Sukompiliuojame kodą ir sukuriame `.exe` failą <br>
   ```
   cmake --build . --config Release
   ```
4. Paleidžiame .exe failus testavimo arba programos<br>
  ```
  .\Antanfontanu.exe
  
  ```
  

**Arba galite pasinaudoti pateiktu `run.bat` failu** <br>
Tada Nukopijuokite projekto direktoriją į savo kompiuterį ir paleiskite `run.bat` failą
  ```
   cd projekto_direktorija
   .\run.bat
  ```


#  Kompiuterio charakteristikos

CPU: AMD Ryzen 5 7535U with Radeon Graphics 2.90 GHz <br>
RAM: 16GB (13.7GB naudojami) <br>
HDD: SSD 512GB (SKHynix HFS512GEJ4X112N) <br>

#  Programos naudojimosi instrukcijos

1. Prieš paleidžiant programą, savo projekte reikia turėti failą:
   "tekstas.txt" – į kurį keliami visi tekstai, kurie gali būti analizuojami.
 

2. Paleidus programą atsidarys meniu su pasirinkimais ar norite skaičiuoti žodžių dažnį, ar nuorodas, ar abu
3. Failas bus nuskaitytas ir automatiškai panaudotas žodžių dažnio, vietos bei nuorodų analizavimui.

4. Visų rezultatų failai bus sukurti aplanke.

---

# Programos funkcijos
## std::unordered_set<std::string> loadTlds(const std::string& filename)

Nuskaito galiojančių domenų (TLD) sąrašą iš teksto failo ir grąžina jį kaip `unordered_set`

## void wordCrossRef(const std::string& filename, const std::string& outFile)

Sudaro cross-reference lentelę: suskaičiuoja žodžių pasikartojimus tekste, nurodo, kuriose teksto eilutėse jie buvo rasti. Rezultatai išvedami į nurodytą išvesties failą.

## void findUrls(const std::string& filename, const std::string& outFile, bool printToScreen, const std::unordered_set<std::string>& tlds)

Ieško visų galimų URL tekste. 

## bool isIgnoredUtf8(unsigned char c)

Patikrina, ar simbolis priklauso ignoruojamiems UTF-8 skyrybos simboliams .

## bool isWordChar(unsigned char c)

Nustato, ar simbolis gali būti laikomas žodžio dalimi

## bool isUrlEnd(unsigned char c)

Nustato, ar simbolis žymi URL adreso pabaigą

## std::string trimUrlPunct(std::string s)

Pašalina skyrybos ženklus URL adreso pabaigoje 

## bool looksLikeDomain(const std::string& s)

Patikrina, ar pateiktas tekstas atitinka domeno vardo struktūrą 

