// Pablo Antonio Bernabeu Cartagena 20083111V

#include<iostream>
#include<fstream>
#include<cstring>
#include<vector>
#include<stdlib.h>
#include<limits>
#include<algorithm>
using namespace std;

const int SENTINEL = -1;


// Función que convierte un string a numero, en la lectura de los archivos
int convertir_a_numero(char cadena){
  int s = cadena-48;
  return s;
}

// Función para leer los archivos y almacenar la información en sus variables
void leerFichero(ifstream &fichero, string &file, int &n, double &T, vector<double> &t, vector<double> &v, vector<int> &m){

  if(fichero.is_open()){
    double aux;

    if(fichero>>n){
      fichero>>T;

      for(int i=0; i<n;i++){
        fichero>>aux;
        t.push_back(aux);
      }

      for(int i=0; i<n;i++){
        fichero>>aux;
        v.push_back(aux);
      }

      for(int i=0; i<n;i++){
        fichero>>aux;
        m.push_back(aux);
      }
    }

    fichero.close();

  }
  else{
    cerr<<"ERROR: cant´t open file: "<<file<<"."<<endl;
    exit(0);
  }
}


// ---------------------------------------------------------------------------------------------------------------------------------
double potter_bt_optimo_c(const vector<double> &v, const vector<double> &w, size_t k, double W){
  vector<unsigned> idx(w.size());
  for(unsigned i=0; i<idx.size(); i++) idx[i]=i;

  sort(idx.begin(), idx.end(),
    [v,w](unsigned x, unsigned y){
      return v[x]/w[x] > v[y]/w[y];
    }
  );

  double acc_v = 0.0;
  double acc_w = 0.0;

  for(unsigned i=1; i<=k; i++){
    if(w[idx[i]]>W){
      acc_v += W/w[idx[i]]*v[idx[i]];
      acc_w += W/w[idx[i]]*w[idx[i]];
      break;
    }
    acc_v += v[idx[i]];
    W -= w[idx[i]];
  }

  return acc_v;
}

double potter_bt_optimo_d(const vector<double> &v, const vector<double> &w, double &W){
  vector<unsigned> idx(w.size());
  for(unsigned i=0; i<idx.size(); i++) idx[i]=i;

  sort(idx.begin(), idx.end(),
    [v,w](unsigned x, unsigned y){
      return (double)v[x]/w[x] > (double)v[y]/w[y];
    }
  );

  double acc_v = 0.0;

  for(auto i : idx){
    if(w[i]<W){
      acc_v += v[i];
      W-=w[i];
    }
  }

  return acc_v;
}

void potter_bt_optimo(const vector<double> &v, const vector<double> &w, double W, size_t k, vector<unsigned> &x,double acc_w, double acc_v, double &best_v){
  if(k==x.size()){
    best_v = max(acc_v, best_v);
    return;
  }

  for(unsigned j=0; j<2; j++){
    x[k]=j;
    double present_w = acc_w + x[k] * w[k];
    double present_v = acc_v + x[k] * v[k];
    if(present_w<=W && present_v + potter_bt_optimo_c(v,w,k+1,W-present_w) > best_v){
      potter_bt_optimo(v, w, W, k+1, x, present_w, present_v, best_v);
    }
  }
}

double potter_bt_optimo(const vector<double> &v, const vector<double> &w, double W, vector<unsigned> &x){

  double best_v = potter_bt_optimo_d(v,w,W);

  potter_bt_optimo(v,w,W, 0, x, 0, 0, best_v);

  return best_v;
}
// ---------------------------------------------------------------------------------------------------------------------------------


// Función principal
int main(int argc, char *argv[]){

  string file="";

  // Gestión de argumentos
  for(int i=1; i<argc; i++){
    if(strcmp(argv[i], "-f")==0){
      if(i<argc-1){
        i++;
        file=argv[i];

        // Si se encuentra la opcion '-f' se procede a leer el fichero.

        int n=0;  double T=0;
        vector<double> t; // vector de tiempos
        vector<double> v; // vector de valores
        vector<int> m; // vector de veces que se repite un elemento

        vector<double> vMejor;  // vector de valores alargado con m
        vector<double> tiemposMejor; // vector de tiempos alargado con m

        ifstream fichero(file);
        leerFichero(fichero, file, n, T, t, v, m);

        for(long unsigned int j=0; j<m.size(); j++){  // Cambiamos las matrices
          for(int k=0; k<m[j]; k++){
            vMejor.push_back(v[j]);
            tiemposMejor.push_back(t[j]);
          }
        }


        // Implementación del nuevo algoritmo.

        vector<unsigned> x(vMejor.size());

        cout<<potter_bt_optimo(vMejor, tiemposMejor, T, x)<<endl;
        
        exit(1);
      }
      else{
        cerr<<"ERROR: missing filename."<<endl;
        exit(0);
      }
    }
    else{
      cerr<<"ERROR: unknown option "<< argv[i] <<"."<<endl;
      cerr<<"Usage: "<<endl;
      cerr<<"potter [-t] [--ignore-naive] -f file"<<endl;
      exit(0);
    }
  }

  return 0;
}
