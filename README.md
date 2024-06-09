POO-TP1
=======

The OOP course assignment #1, CS UFOP.

## The Assignment

The target is a system for the resource management of a mechanic's workshop.
In it, it is stated that there will be 4 different agents over the application:

1. The workshop staff, that can be either sellers or the mechanics themselves;
2. the clients;
3. the vehicles from the clients;
4. and the selling itens and services from it. 

In the system, for the workshop to budget or execute a maintenance service, the clients and their vehicles must first be registered.

On the client attendance, in case of he doesn't having it yet, a registry of he and its vehicles in the workshop shall be provided. It will be followed by a service order request to budget or to maintenance. The order must be taken with information as the maintenance reason, as well as the mileage of the vehicle; at this step, the entire history of the vehicle's maintenance must be available.

At the mechanic's side, he must be able seeing all service orders in open to select one to be done. In case of the service being simply the budget, it service must not be executed until previous authorization from the part of the client. The seller must be able seeing these orders that are pendent authorization and contact the clients for verifying approval. If the clients denies the order, then it should be closed.

The mechanic is supposed to detail all the maintenance services done, together with their respective prices, and to list all the parts, with its price and labor price.

(...)


## The Implementation

Inherently attached to the assignment is the use of OOP. For that, C++ is used.


## Building

The project's system is built using makefile. On installation, the system's pre-requisites are:

* Architecture x86 or amd64;
* GNU Makefile and GCC, as the C++ compiler.

For then one runs
> make build


### Other Specifications

* The project uses the standard C++11.


