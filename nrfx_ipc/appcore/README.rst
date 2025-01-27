.. _nrfx_ipc:

nrfx_ipc example
################

Overview
********

This sample demonstrates the usage of nrfx_ipc driver directly.

The application core sends messages to network core in a while 
loop, while the network core sends messages back to application 
core in a while loop too.

Both application core and network core would print out the messages
they received.

Requirements
************

The following development kits are tested for this sample.

+------------------------------------------------------------------+
|Build target                                                      +
+==================================================================+
|nrf5340dk_nrf5340_cpuapp                                          |
+------------------------------------------------------------------+

The following NCS tags are tested for this sample. By default, NCS ``v1.9.1`` is used.

+------------------------------------------------------------------+
|NCS tags                                                          +
+==================================================================+
|v1.9.x                                                            |
+------------------------------------------------------------------+

Build & Programming
*******************

For example, enter the following command to build ``nrf5340dk_nrf5340_cpuapp``.

.. code-block:: console

   west build -b nrf5340dk_nrf5340_cpuapp -d build_nrf5340dk_nrf5340_cpuapp -p

To flash the images to the board, use the following command:

.. code-block:: console

   west flash -d build_nrf5340dk_nrf5340_cpuapp

Testing
*******

Program application core with nrfx_ipc/appcore and network core with nrfx_ipc/netcore.
Test it by performing the following steps:

1. Connect the kit to the computer using a USB cable. The kit is assigned a COM port (Windows) or ttyACM device (Linux), which is visible in the Device Manager.
#. |connect_terminal|
#. Open application core correspondent COM port
#. The IPC dual core communication starts to work 

Once the communication is successful, the logging looks similar to the following output.

.. code-block:: console

	*** Booting Zephyr OS build v2.7.99-ncs1  ***
   <inf> ipc_app: dual core communication sample at 09:36:24 May 31 2022
   <inf> ipc_app: ipc init done
   <inf> ipc_app: app core start to send
   <inf> ipc_app: sent successfully 0
   <inf> ipc_app: event_mask 2
   <inf> ipc_app: Received: 
    49 20 61 6d 20 66 72 6f  6d 20 4e 45 54 20 00 00 
   <inf> ipc_app: app core start to send
   <inf> ipc_app: sent successfully 1
   