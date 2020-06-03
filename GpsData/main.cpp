#include <stdio.h>
#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <queue>
#include <string>
#include <sstream>
#include <windows.h>

using namespace std;
//Jiri Liska, PRF JCU CZ, 2015
//Placemark je jednotlivy bod, Placemarky jsou razene do Folder (coz jsou pole)
//Folder ma name a placemarky
//pole Folderu je latifundie

//Placemark
class Placemark{
		string name;
		float longitude;
		float latitude;
		float altitude;
		float heading;
		float tilt;
		float range;
public:
		Placemark(float longitude, float latitude, string name){
			this ->longitude=longitude;
			this ->latitude=latitude;
			this ->name=name;
			altitude=440;
			heading=0;
			tilt=0;
			range=100;
		}
		
		void zapisPlacemark(ofstream &outf){
			outf << "<Placemark>" << endl;
			outf << "<name>" << name << "</name>" << endl;
			outf << "<LookAt>" << endl;
			outf << "<longitude>" << longitude << "</longitude>" << endl;
			outf << "<latitude>" << latitude << "</latitude>" << endl;
			outf << "<altitude>" << altitude << "</altitude>" << endl;
			outf << "<heading>" << heading << "</heading>" << endl;
			outf << "<tilt>" << tilt << "</tilt>" << endl;
			outf << "<range>" << range << "</range>" << endl;
			outf << "<gx:altitudeMode>" << "relativeToSeaFloor" << "</gx:altitudeMode>" << endl;
			outf << "</LookAt>" << endl;
			outf << "<styleUrl>" << "#msn_placemark_circle" << "</styleUrl>" << endl;
			outf << "<Point>" << endl;
			outf << "<coordinates>" << longitude << "," << latitude << "," << altitude << "</coordinates>" << endl;
			outf << "</Point>" << endl;
			outf << "</Placemark>" << endl;
		}
		
		void VypisPlacemark(){
			cout << "<Placemark>" << endl;
			cout << "<name>" << name << "</name>" << endl;
			cout << "<LookAt>" << endl;
			cout << "<longitude>" << longitude << "</longitude>" << endl;
			cout << "<latitude>" << latitude << "</latitude>" << endl;
			cout << "<altitude>" << altitude << "</altitude>" << endl;
			cout << "<heading>" << heading << "</heading>" << endl;
			cout << "<tilt>" << tilt << "</tilt>" << endl;
			cout << "<range>" << range << "</range>" << endl;
			cout << "<gx:altitudeMode>" << "relativeToSeaFloor" << "</gx:altitudeMode>" << endl;
			cout << "</LookAt>" << endl;
			cout << "<styleUrl>" << "#msn_placemark_circle" << "</styleUrl>" << endl;
			cout << "<Point>" << endl;
			cout << "<coordinates>" << longitude << "," << latitude << "," << altitude << "</coordinates>" << endl;
			cout << "</Point>" << endl;
			cout << "</Placemark>" << endl;
		}
        
    void predejSouradnicePlacemarku(float &lon,float &lat) {
        lon=longitude;
        lat=latitude;
    }
	
};

//Folder
class Folder{
	string name;	
	vector<Placemark> folderPlacemarku;
	
public:
		Folder(string name){
			this ->name=name;
		}

       ~Folder(){
            folderPlacemarku.clear();
       }
 		
		virtual vector<Placemark>::iterator ZaradPMdoFolderu(Placemark pm){
			folderPlacemarku.push_back(pm);
			return folderPlacemarku.end();
			//zaradi Placemark do folderu a vrati ukazatel na konec vektoru 
			//iterator zatim nepotrebujeme
			
		}
		
	
		
		virtual void zapisFolder(ofstream &outf){
		vector<Placemark>::iterator it;
		for(it=folderPlacemarku.begin(); it!=folderPlacemarku.end(); it++){
			outf << "<Folder>" << endl;
			outf << "<name>" << name << "</name>" << endl;
			it->zapisPlacemark(outf);
			outf << "</Folder>" << endl;
		}
		}
		
		virtual string Foldername(){
			return name;
		}

	void vypisFolder(){
		vector<Placemark>::iterator it;
		for(it=folderPlacemarku.begin(); it!=folderPlacemarku.end(); it++){
			cout << "<Folder>" << endl;
			cout << "<name>" << name << "</name>" << endl;
			it->VypisPlacemark();
			cout << "</Folder>" << endl;
		}
		}
        


    
};


class FolderInterpolovany: public Folder {
	string name;	
	vector<Placemark> folderPlacemarku;
    vector<Placemark> folderPlacemarkuI;
public:
	static vector<Placemark>::iterator itfi;
       //dedeni konstruktoru viz http://kmlinux.fjfi.cvut.cz/~fabiadav/cecko/poznamky-k-jazyku-c_plus_plus/zaklady-dedicnosti
	FolderInterpolovany(string name): Folder(name) {}

        ~FolderInterpolovany(){
            folderPlacemarku.clear();
            folderPlacemarkuI.clear();
       }

 
	virtual vector<Placemark>::iterator ZaradPMdoFolderu(Placemark pmi){
	FolderInterpolovany::folderPlacemarku.push_back(pmi);
	itfi=FolderInterpolovany::folderPlacemarku.end()-1;
			if(itfi==(FolderInterpolovany::folderPlacemarku.begin())) {
                
                return itfi;
            } //z jednoho bodu nelze interpolovat
            
           
			float lon1, lat1, lon2, lat2;
            itfi->predejSouradnicePlacemarku(lon1, lat1);
            (itfi-1)->predejSouradnicePlacemarku(lon2, lat2);
			 
			
			try{
	vlozMezibod(lon1, lat1, lon2, lat2);
			}
			catch(...){
				cout << "CHYBA DLL KNIHOVNY NEBO INTERPOLACE, ASI CHYBI haversine.dll" << endl;
				cout << "vynechavam vkladani pomocnych bodu" << endl;
			}
	
	return itfi;
	//zaradi Placemark do folderu a vrati ukazatel na konec vektoru 
	//iterator zatim nepotrebujeme
		
	}



//vlozeni mezibodu
   void vlozMezibod(float lon1, float lat1, float lon2, float lat2){
        //nacteni knihovny
    string libName = "haversine.dll";
    string fName;
        HMODULE handle = LoadLibrary(libName.c_str());
	if (handle == NULL){
                //kdyz se knihovnu nepodari nacist
                cout << "Knihovnu " << libName << " se nepodarilo inicializovat!" << endl; 
                throw "problem s nactenim knihovny"; }
	   
             fName="havdistance"; //vytvoreni ukazatele fdist
	   typedef double funkce(double, double, double, double);
 	       funkce *fdist = (funkce*)GetProcAddress(handle,fName.c_str());
        if (fdist == NULL){	 cout << "problem pri nacteni funkce fdistance" << endl; throw "problem pri nacteni funkce fdistance";}
            fName="midpoint"; //vytvoreni ukazatele fmidp
			//predava se midpoint(latit1,longit1,latit2,longit2,latitmid, longitmid)  - je to prohozene tak bacha
         typedef void funkce2(double, double, double, double, double&, double&);
           funkce2 *fmidp = (funkce2*)GetProcAddress(handle,fName.c_str());
       if (fmidp == NULL){	 cout << "problem pri nacteni funkce midpoint" << endl; throw "problem pri nacteni funkce midpoint";}

   
   //vlastni pouziti
       if((*fdist)(lat1, lon1, lat2, lon2) > 5){ //pravidlo pro rekurzi (5metru), kdyz nevyhovi, ukonci se rekurzni retezec
           double latmdp, lonmdp;
		   fmidp(lat1,lon1,lat2,lon2,latmdp,lonmdp);
		 //  cout << latmdp << "   " << lonmdp << endl; //pro LADENI
		   string name="";
		  FolderInterpolovany::folderPlacemarkuI.push_back(Placemark(lonmdp, latmdp, name));
		   //rekurze, vola se vzdy na jednu a druhou stranu od midp
           vlozMezibod(lon2, lat2,lonmdp, latmdp);
           //a na druhou stranu
           vlozMezibod(lon1, lat1,lonmdp, latmdp);
		  
		
       }
       else {
    // uvolneni knihovny z pameti
	   FreeLibrary(handle); //tady konci blok pouziti knihovny
	   return;}
     
   }
      
    

		virtual void zapisFolder(ofstream &outf){
		vector<Placemark>::iterator it;
		for(it=folderPlacemarku.begin(); it!=folderPlacemarku.end(); it++){
			outf << "<Folder>" << endl;
			outf << "<name>" << name << "</name>" << endl;
			it->zapisPlacemark(outf);
			outf << "</Folder>" << endl;
		}
        for(it=folderPlacemarkuI.begin(); it!=folderPlacemarkuI.end(); it++){
			outf << "<Folder>" << endl;
			outf << "<name>" << name << "</name>" << endl;
			it->zapisPlacemark(outf);
			outf << "</Folder>" << endl;
		}
		} 

};

//Latifundie
vector<Folder*> latifundie;
vector<Folder*>::iterator it_latifundie;
//Latifundie interpolovana
vector<Folder*> latifundieInterpolovana;
vector<Folder*>::iterator it_latifundieInterpolovana;


//infq je fronta, kde je ulozen po radkach vstupni file s daty, co radek to string
queue<string, list<string> > infq;
//infq je GLOBALNI promenna
void nactifile(queue<string, list<string> > &infqa){
string vststring;
ifstream infile;
infile.open("indata.txt");
if(!infile){
	cout << "!! musi existovat soubor indata.txt v aktualni slozce!" << endl;
	throw("neexistujici soubor indata.txt");
}
while (!infile.eof())
	{
	getline(infile,vststring);
	infqa.push(vststring);
	vststring.clear();
	}

infile.close();

}

/*
//vypisfrontu je kvuli ladeni, vypise obsah fronty (nacteneho file)
void vypisfrontu(queue<string, list<string> > &infqa){
string vststring;

	while(!infqa.empty()){
	//vypis prvniho prvku
	cout << infqa.front() << endl;
	//postupna "likvidace" fronty
	infqa.pop();
	}
}
*/



// predej radek funkce neni soucasti zadneho objektu, preda radek ze zasobnikove fronty a ukousne ji
string predejRadek(queue<string, list<string> > &infqa){
string radek;

	if(infqa.empty()) return "NULL";
	//vypis prvniho prvku
	radek= infqa.front();
	//postupna "likvidace" fronty
	infqa.pop();
	return radek;
}

//staticky ukazatel na posledni prvek
vector<Placemark>::iterator FolderInterpolovany::itfi;







int main(int argc, char **argv)
{
cout << "//Jiri Liska, PRF JCU CZ, 2015" << endl;
cout << " program prepisuje data ze vstupniho souboru \"indata.txt\" do \n vystupniho vystup.kml (standartni xml format)" << endl;
cout << "a souboru vystupint.kml, kde jsou body vcetne doplnenych bodu \n mezi existujici body" << endl;
cout << " indata.txt ma format :cislopozemku (nebo nazev, rozlisujici znak \n je : na zacatku radku)" << endl;
cout << " na dalsich radcich nasleduji souradnice jednotlivych bodu ve \n formatu latitude, longitude" << endl;

try{
nactifile(infq); //vstupni soubor se presune do fronty infq
}
catch(...){
	cout << "!! musi existovat soubor indata.txt v aktualni slozce!" << endl;
	return 99;
}


//latifundie bude jen jedna
//it_latifundie je iterator na aktualni-posledni prvek vektoru latifundie

string sradek;
string aktualnipozemek;
//ukousne a preda 1 radek do string sradek
//cyklus tu zacne
int rozlisenipoz;
float longitudeprvni=0, latitudeprvni=0;
string name="";
for(int i=0;;i++){
	//ukousne a preda 1 radek do string sradek
sradek=predejRadek(infq);




	if (sradek=="NULL" || sradek=="" || sradek==" " ){	//konec vstup. file
        (*it_latifundieInterpolovana)->ZaradPMdoFolderu(Placemark(longitudeprvni, latitudeprvni, name));
    break;}
	

//obsah radku je jak v string sradek, tak v stringstream ssradek
//kdyz radek zacina dvojteckou je to uvozeni dalsiho folderu, nasleduji placemarky do dalsi dvojtecky
//kdyz jsou na radku souradnice, je to longitude carka mezera latitude

if ((*sradek.begin())==':'){
	rozlisenipoz=0;
	latifundie.push_back(new Folder(sradek));
    //aby se interpolovala posledni strana, je treba poslat prvni prvek znovu pri ukoncovani folderu
    if(name!=""){(*it_latifundieInterpolovana)->ZaradPMdoFolderu(Placemark(longitudeprvni, latitudeprvni, ""));}
	latifundieInterpolovana.push_back(new FolderInterpolovany(sradek)); //interpolovane
	aktualnipozemek=sradek;
	it_latifundie=latifundie.end()-1; //it_latifundie je ted ukazatel na aktualni folder
	it_latifundieInterpolovana=latifundieInterpolovana.end()-1;
	//pro ladeni:
	cout << "zarazovani Folderu" << endl;	

	}
	else	//v sradek je souradnice
	{
	unsigned it_sradek;
	rozlisenipoz++;
	it_sradek=sradek.find(",");
	string slongitude, slatitude;
	slatitude=sradek.substr(0,it_sradek);
	slongitude=sradek.substr(it_sradek+1);
	stringstream sslongitude(slongitude);
	stringstream sslatitude(slatitude);
	float longitude,latitude;
	sslongitude >>longitude;
	sslatitude>>latitude;
    if(rozlisenipoz==1){longitudeprvni=longitude; latitudeprvni=latitude;} //kvuli interpolaci od posledniho k prvnimu
	
	stringstream pomocna;
	pomocna << rozlisenipoz; //udela z rozlisenipoz string
	name=(aktualnipozemek+"_"+pomocna.str());
	(*it_latifundie)->ZaradPMdoFolderu(Placemark(longitude, latitude, name));
	(*it_latifundieInterpolovana)->ZaradPMdoFolderu(Placemark(longitude, latitude, name));

	} //konec else

}
//konec for cyklu   - konci break kdyz vyprazdni zasobnikovou frontu
//v teto chvili je naplnen zasobnik latifundie



ofstream outfs;
outfs.open("vystup.kml");
//zapise hlavicku souboru
outfs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
outfs << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">" << endl;
outfs << "<Document>" << endl;
outfs.precision(11);
for(it_latifundie=latifundie.begin();it_latifundie!=latifundie.end();it_latifundie++){
	(*it_latifundie)->zapisFolder(outfs);
    delete *it_latifundie;
}
outfs << "</Document>" << endl;
outfs << "</kml>" << endl;
outfs.close();
latifundie.clear();

//jeste zapis interpolovaneho
ofstream outfsi;
outfsi.open("vystupint.kml");
//zapise hlavicku souboru
outfsi << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
outfsi << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">" << endl;
outfsi << "<Document>" << endl;
outfsi.precision(11);
for(it_latifundieInterpolovana=latifundieInterpolovana.begin();it_latifundieInterpolovana!=latifundieInterpolovana.end();it_latifundieInterpolovana++){
	//cout << "pruchod zapis interpolovana" << endl;
(*it_latifundieInterpolovana)->zapisFolder(outfsi); 
delete (*it_latifundieInterpolovana);
                        }
outfsi << "</Document>" << endl;
outfsi << "</kml>" << endl;
outfsi.close();
latifundieInterpolovana.clear();
}

	
	
	
