#include <iostream>
#include <winsock2.h>
#include <thread>
#include <cstring>
#include <ws2tcpip.h>
#include <stdio.h>

using namespace std;

int puerto = 0;

class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    Client()
    {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        cout<<"Conectando al servidor..."<<endl<<endl;
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(puerto);
        iResult = connect(server, (SOCKADDR *)&addr, sizeof(addr));
        if (iResult == SOCKET_ERROR) {
            wprintf(L"No se pudo conectar, error: %ld\n", WSAGetLastError());
            iResult = closesocket(server);
            if (iResult == SOCKET_ERROR)
                wprintf(L"No se pudo cerrar el socket, error: %ld\n",WSAGetLastError());
        WSACleanup();
        system("pause");
        exit(EXIT_SUCCESS);
        }
        cout << "Conectado al Servidor!" << endl;
    }
    void Enviar(string mensaje)
    {
        strcpy(buffer, mensaje.c_str());
        send(server, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    string Recibir(){
        recv(server, buffer, sizeof(buffer), 0);
        string mensaje (buffer);
        memset(buffer, 0, sizeof(buffer));
        return mensaje;
    }

    void CerrarSocket()
    {
       closesocket(server);
       WSACleanup();
       cout << "Socket cerrado.\n";
    }
};






//RETORNO:
//0 no hay errores
//1 El largo del mensaje supera los no esta entre los 1 a 20 caracteres
//2 El mensaje tiene simbolos ocupados (A B C D)
//3 el mensaje es "volver" sin diferenciar mayusculas o minusculas
int verificarMensaje (string mensaje){

    int resultado = 0;
    if (mensaje.length()>20||mensaje.length()<1){
        resultado = 1;
    }
    else if ((mensaje.find_first_of("ABCD"))!=-1){
        resultado = 2;

    } else if(strcasecmp(mensaje.c_str(), "volver")==0){
        resultado = 3;
    }

    return resultado;
}




int main()
{
    cout << "Introducir Puerto de conexion\n";

    cin >> puerto;

    Client *Cliente = new Client();

    system("pause");

    boolean loop = true;
    int menu = 0;
    string mensaje = " ";
    int verificacion = 0;
    int eof = 1;

    while(loop){
        switch (menu){
            case 1:

                system("cls");

                cout << "\n******CALCULADORA******\n\nEscribi \"volver\" para regresar al menu.\nIntroducir calculo:\n";
                cin >> mensaje;

                verificacion = verificarMensaje(mensaje);

                switch (verificacion){

                    case 1:
                        cout << "ERROR la cantidad de caracteres no esta entre los 1 y 20 permitidos\n";
                        system("pause");
                    break;

                    case 2:
                        cout << "ERROR el mensaje tiene caracteres prohibidos, los caracteres prohibidos son:\nA B C D\n";
                        system("pause");
                    break;

                    case 3:
                        menu = 0;
                    break;

                    default:
                        Cliente->Enviar("A"+mensaje);
                        mensaje = Cliente->Recibir();
                        if (!mensaje.empty()){
                            cout << "Respuesta:\n" << mensaje << endl << endl;
                            system("pause");
                        }
                        else {
                            cout << "\nEl mensaje no se envio, se corto la conexion por inactividad\nSe va a cerrar el programa\n\n";
                            system("pause");
                            exit(EXIT_SUCCESS);
                        }
                    break;
                }
            break;

            case 3:
                Cliente->Enviar("B");
                Cliente ->CerrarSocket();
                loop = false;
            break;

            case 2:
                system("cls");
                Cliente->Enviar("C");
                mensaje = Cliente->Recibir();
                if (!mensaje.empty()){
                    while (eof == 1){
                        mensaje = Cliente->Recibir();
                        if (mensaje[0]=='C'){
                            eof = 0;
                        }
                        else{
                            cout << mensaje << endl;
                        }
                }
                eof = 1;
                system("pause");
                }
                else {
                    cout << "\nEl mensaje no se envio, se corto la conexion por inactividad\nSe va a cerrar el programa\n\n";
                    system("pause");
                    exit(EXIT_SUCCESS);
                }
                menu = 0;
            break;

            default:
                system("cls");
                cout << "**************MENU**************\n\n Elegir funcion\n 1- Usar calculadora\n 2- Ver registro de actividades\n 3- Cerrar Sesion\n";
                cin >> menu;
            break;

        }
    }
}
