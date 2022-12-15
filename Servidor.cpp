#include <iostream>
#include <winsock2.h>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ctime>
#include <thread>


using namespace std;

#define PUERTO 5000


bool esperando = false;
bool conexion = true;



class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    Server()
    {

        ofstream log;

        log.open("servidor.log", fstream::app);

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        log << std::put_time(&tm, "%d-%m-%Y %H:%M:%S")<< ": ======================================"<< endl;
        log << std::put_time(&tm, "%d-%m-%Y %H:%M:%S")<< ": ==========Inicia el Servidor=========="<< endl;
        log << std::put_time(&tm, "%d-%m-%Y %H:%M:%S")<< ": ======================================"<< endl;

        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PUERTO);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes en el pueto: "<< PUERTO << endl;

        t = std::time(nullptr);
        tm = *std::localtime(&t);
        log << std::put_time(&tm, "%d-%m-%Y %H:%M:%S")<< ": Socket creado. Puerto de escucha: "<< PUERTO << endl;

        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize))!= INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;

        t = std::time(nullptr);
        tm = *std::localtime(&t);
        log << std::put_time(&tm, "%d-%m-%Y %H:%M:%S")<< ": Conexion aceptada" << endl;

        }

        log.close();
    }

    void EsperarCliente()
    {
        ofstream log;

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes." << endl;

        log.open("servidor.log", fstream::app);

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        log << std::put_time(&tm, "%d-%m-%Y %H:%M:%S")<< ": Esperando cliente, Puerto de escucha: "<< PUERTO << endl;

        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize))!= INVALID_SOCKET)
        {

        cout << "Cliente conectado!" << endl;

        t = std::time(nullptr);
        tm = *std::localtime(&t);
        log << std::put_time(&tm, "%d-%m-%Y %H:%M:%S")<< ": Conexion aceptada" << endl;


        }
        log.close();
    }

    string Recibir(){
        recv(client, buffer, sizeof(buffer), 0);
        string mensaje(buffer);
        memset(buffer, 0, sizeof(buffer));
        return mensaje;
    }


    void Enviar(string mensaje)
    {
        strcpy(buffer, mensaje.c_str());
        send(client, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    void CerrarSocket()
    {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado.\n" << endl;
    }
};

//Defino procedimiento de timeout para el thread

void TimeOut(Server* servidor)
{
    int tiempo = 0;
    while (esperando){
        Sleep(1000);
        tiempo++;
        if (tiempo>=120){
            cout << "\nTimeout, se va a cerrar la conexion con el cliente\n";
            servidor->CerrarSocket();
            conexion = false;
        }
    }
}


//RETORNO:
//-1 no hay errores
//Si el mensaje tiene simbolos fuera del diccionario se devuelve el numero de la posición del error
int ValidarCaracteres (string mensaje){

    string diccionario = "0123456789+-*/^!";
    int resultado = -1;

    for (int i=0;i<mensaje.length();i++){
        if ((diccionario.find_first_of(mensaje[i]))==-1){
            resultado = i;
            break;
        }
    }
    return resultado;
}

//RETORNO:
//-1 no hay errores
//-2 no hay operador
//Si hay un error en el formato de la opercaión se devuelve el numero de la posición del error
int ValidarOperacion (string mensaje){

    string numeros = "0123456789";
    string operadores = "+-*/^!";
    int resultado=-1;

    //se busca la posicion del operador
    int posOperador = mensaje.find_first_of(operadores);

    //verifico si hay un operador
    if (posOperador == -1){
        resultado = -2;
    } //verifico que el operador no este al principio
    else if (posOperador==0){
        resultado = posOperador;
    } //diferencio si el operador es factorial o no
    else if (mensaje[posOperador]!='!'){
        //verifico si el operador esta al final
        if (posOperador==mensaje.length()-1){
            resultado = posOperador;
        } //verifico si los caracteres despues del operador son numeros
        else{
          for (int i=posOperador+1;i<mensaje.length();i++){
            if (numeros.find(mensaje[i])==-1){
                resultado = i;
                break;
            }
          }
        }
    } //verifico que el operador factorial este al final
    else if (posOperador!=mensaje.length()-1){
        resultado = posOperador;
    }
    return resultado;
}

int realizarCalculo(string formula){

    string operadores = "+-*/^!";
    int resultado=1;
    int posOperador = formula.find_first_of(operadores);
    char operador = formula[posOperador];

    int operando1 = std::stoi(formula.substr(0,posOperador));

    if (operador == '!'){
        for (int i=1;i<=operando1;i++){
            resultado = resultado * i;
        }
    }
    else {
        int operando2 =
        std::stoi(formula.substr(posOperador+1,formula.length()-posOperador-1));
        if (operador == '+'){
            resultado = operando1 + operando2;
        }
        else if (operador == '-'){
            resultado = operando1 - operando2;
        }
        else if (operador == '*'){
            resultado = operando1 * operando2;
        }
        else if (operador == '/'){
             resultado = operando1 / operando2;
        } else{
            resultado = pow(operando1,operando2);
        }

    }
    return resultado;
}


//RETORNO:
//devuelve un string con el resultado
//En caso de un error en la validación devuelve un mensaje indicando el error
string ResponderCalculo(string formula){

    string resultado = " ";
    int validacion = ValidarCaracteres(formula);
    std::stringstream stream;

    if (validacion!=(-1)){
        resultado ="No se pudo realizar la operacion, Se encontro un caracter no contemplado: [" + string(1,formula[validacion]) + "]";
    } else{
        validacion = ValidarOperacion(formula);
        switch (validacion){
            case -1:
                resultado = to_string(realizarCalculo(formula));
            break;
            case -2:
                resultado=formula;
            break;
            case 0:
                resultado = "No se pudo realizar la operacion, la operacion esta mal formada: [" + string(1,formula[validacion]) + string(1,formula[validacion+1]) + "]";
            break;
            default:
                if (validacion==formula.length()-1){
                    resultado ="No se pudo realizar la operacion, la operacion esta mal formada: [" + string(1,formula[validacion-1]) + string(1,formula[validacion]) + "]";
                }
                else{
                    resultado ="No se pudo realizar la operacion, la operacion esta mal formada: [" + string(1,formula[validacion-1]) + string(1,formula[validacion]) + string(1,formula[validacion+1]) + "]";
                }
        }
    }
    return resultado;
}


int main()
{

    //ESPERO LA CONEXION
    Server *Servidor = new Server();

    ifstream logLectura;
    ofstream logEscritura;

    auto t =std::time(nullptr);
    auto tm = *std::localtime(&t);
    string mensaje = " ";





    //INICIO EL BUCLE PRINCIPAL
    while(true){

        // se abre el archivo para registrar mientras dure la conexion


        //Entro en el bucle de conexion con el cliente
        while (conexion){


            logEscritura.open("servidor.log", ios::app);
            t = std::time(nullptr);
            tm = *std::localtime(&t);
            logEscritura << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << ": Esperando recibir mensaje" << endl;
            logEscritura.close();

            //Inicio el thread del timeout y le aviso mediante [esperando] que el sistema esta en espera de recibir mensaje
            thread timeOut(TimeOut, Servidor);
            esperando = true;

            mensaje = Servidor->Recibir();

            //se avisa al timeOut que ya se recibio mensaje y se espera a que termine de ejecutar el hilo
            esperando = false;
            timeOut.join();

            logEscritura.open("servidor.log", ios::app);
            t = std::time(nullptr);
            tm = *std::localtime(&t);
            logEscritura << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << ": Mensaje Recibido: " << mensaje << endl;
            logEscritura.close();

            //diferencio segun el comando que indica el mensaje
            if (mensaje[0] == 'A'&&conexion){

                mensaje = mensaje.erase(0,1);

                mensaje = ResponderCalculo(mensaje);
                Servidor ->Enviar(mensaje);

            }
            else if (mensaje[0] == 'B'&&conexion){

                Servidor->CerrarSocket();
                conexion = false;

                logEscritura.open("servidor.log", ios::app);

                t = std::time(nullptr);
                tm = *std::localtime(&t);
                logEscritura << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << ": Sesion con cliente cerrada" << endl;

                logEscritura.close();
            }
            else if(mensaje[0] == 'C'&&conexion){
                Servidor->Enviar("C");
                logLectura.open("servidor.log") ;
                while (!logLectura.eof()){
                    getline (logLectura, mensaje);
                    Servidor->Enviar(mensaje);
                }
                Servidor->Enviar("C");
                logLectura.close();
            }
            else if(conexion)
            {

                cout << "ERROR no se reconoce el comando: " << mensaje;
            }
            else {
                logEscritura.open("servidor.log", ios::app);
                t = std::time(nullptr);
                tm = *std::localtime(&t);
                logEscritura << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << ": Conexión con el cliente Cerrada por Inactividad." << endl;
                logEscritura.close();
           }
        }

        //al terminar la conexion con el cliente vuelco a esperar conexion
        Servidor->EsperarCliente();
        conexion = true;
    }


}
