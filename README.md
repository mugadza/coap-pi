# CoAP protocol with tinydtls on Raspberry Pi 3B+ 

This project was tested on a Raspberry Pi 3B+. It uses the wiringPi library to configure the ISR and the LED on the board.
The final implementations of the project is devided into two parts:

- Implementation of CoAP server with no DTLS - This is on the branch `final/coap-server-no-dtls`
- Implementation of CoAP server with DTLS - This is on the branch `final/coap-server-with-dtls`

To run each implementation, you will need to checkout each branch.

## Setting up the Raspberry Pi 3B+
![Raspberry Pi 3B+ pinout](GPIO-Pinout-Diagram-2.png)
The raspberry pi was setup with PIR sensor for motion detection, and an LED to indicate the state of the sensor. The connection
of this componets to the Pi was is as follows

#### PIR connection to the Pi
| PIR Sensor    | Raspberry Pi  |
| ------------- | ------------- |
| -ve pin       | Ground        |
| +ve pin       | 5V pin        |
| data pin      | GPIO 4        |

#### LED connection to the Pi
The LED should always have a current limiting resistor connected to it.
| LED           | Raspberry Pi  |
| ------------- | ------------- |
| -ve pin       | Ground        |
| +ve pin       | GPIO 17       |

## Running CoAP server with no dtls
Assuming you have cloned this repo on your Pi or unzipped it and you are on the root of this project in your terminal, 
you need to checkout the branch appropriate branch:

- `git checkout final/coap-server-no-dtls`

In order to be able to compile this branch you will need the following libraries:
- [libcoap-1-0](https://ubuntu.pkgs.org/18.04/ubuntu-universe-amd64/libcoap-1-0-bin_4.1.2-1_amd64.deb.html)
- [wiringPi](http://wiringpi.com/download-and-install/)

You can then compile and run the server using:

```
make server
make run_server
```
if you have installed the [Copper4Cr](https://github.com/mkovatsc/Copper4Cr) coap client on your chrome browser, 
you can use it to communicate with the server.

## Running CoAP server with dtls
Continuing from the above setup, to run the server implementation with DTLS, we can checkout the apppropriate branch.
- `git checkout final/coap-server-with-dtls`

For this implementation, the dtls source code is included in the source files hence there will be no new installation needed.
This branch still uses the [wiringPi](http://wiringpi.com/download-and-install/) library to communicate with the Pi.

#### Running the server
The implementation of the server is in the folder *dtls\_server* on this branch. You can compile and run the server using:
```
cd dtls_server
make
./alarm_server 127.0.0.1 8000 
```
As seen above, running the server takes the format `./alarm_server <host> <port>`

#### Running the client
The implementation of the client is in the folder *dtls\_client* on this branch. You can compile and run the client using:

```
cd dtls_client
make
./alarm_client 127.0.0.1 8000 POST enabled 1
```
The above is an example of running the client to enable the alarm. A client takes the following format 
`./alarm_client <host> <port> <method> <resource> <payload>`. 

- _host_ - host name or ip address of the client
- _port_ - port number of the client
- _method_ - GET or POST. The current implementation only supports this two methods
- _resource_ - The name of the resource to run on the server. currently the server has **status** and **enabled**.
- _payload_ - This is only required when the method is POST. Payload can only be 1 or 0 which is intuitive based 
on the resource you are running i.e 1 for method **enabled** means we are enabling the alarm.




