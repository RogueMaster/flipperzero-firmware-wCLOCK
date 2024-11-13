## Overview

**Web Crawler** is a custom application designed for the Flipper Zero device, allowing users to configure and manage HTTP requests directly from their Flipper Zero.

## Requirements
- WiFi Dev Board or Raspberry Pi Pico W for Flipper Zero with FlipperHTTP Flash: https://github.com/jblanked/FlipperHTTP
- WiFi Access Point


## Installation
- Download from the Official Flipper App Store: https://lab.flipper.net/apps/web_crawler
- Video tutorial: https://www.youtube.com/watch?v=lkui2Smckq4

## Features
- **Configurable Request**: Specify the URL of the website you want to send a HTTP request to or download (tested up to 427Mb)
- **Wi-Fi Configuration**: Enter your Wi-Fi SSID and password to enable network communication.
- **File Management**: Automatically saves and manages received data on the device's storage, allowing users to view, rename, and delete the received data at any time.

## Usage
1. **Connection**: After installing the app, turn off your Flipper, connect the WiFi Dev Board, then turn your Flipper back on.

2. **Launch the Web Crawler App**: Navigate to the Apps menu on your Flipper Zero, select GPIO, then scroll down and select **Web Crawler**.

3. **Main Menu**: Upon launching, you'll see a submenu containing the following options:
   - **Run**: Initiate the HTTP request.
   - **About**: View information about the Web Crawler app.
   - **Settings**: Set up parameters or perform file operations.

4. **Settings**: Select **Settings** and navigate to WiFi Settings. Use the Flipper Zero's navigation buttons to input and confirm your settings. Do the same for the Request settings. Once configured, these settings will be saved and used for subsequent HTTP request operations.

   For testing purposes:
   - https://httpbin.org/get Returns GET data.
   - https://httpbin.org/post Returns POST data.
   - https://httpbin.org/put Returns PUT data.
   - https://httpbin.org/delete Returns DELETE data.
   - https://httpbin.org/bytes/1024 Returns BYTES data (DOWNLOAD method)
   - https://proof.ovh.net/files/1Mb.dat Returns BYTES data (DOWNLOAD method - it can download the whole file)
   - https://proof.ovh.net/files/10Mb.dat Returns BYTES data (DOWNLOAD method - it can download the whole file)

5. **Running the Request**: Select **Run** from the main submenu to start the HTTP request process. The app will:
   - **Send Request**: Transmit the HTTP request via serial to the WiFi Dev Board.
   - **Receive Data**: Listen for incoming data.
   - **Store Data**: Save the received data to the device's storage for later retrieval.
   - **Log**: Display detailed analysis of the operation status on the screen.

6. **Accessing Received Data**: After the HTTP request operation completes, you can access the received data by either:
   - Navigating to File Settings and selecting Read File (preferred method)
   - Connecting to Flipper and opening the SD/apps_data/web_crawler_app/ storage directory to access the received_data.txt file (or the file name/type customized in the settings).

## Setting Up Parameters
1. **Path (URL)**
   - Enter the complete URL of the website you intend to crawl (e.g., https://www.example.com/).

2. **HTTP Method**
   - Choose between GET, POST, DELETE, PUT, and DOWNLOAD.

3. **Headers**
   - Add your required headers to be used in your HTTP requests

4. **Payload**
   - Type in the JSON content to be sent with your POST or PUT requests.

5. **SSID (WiFi Network)**
   - Provide the name of your WiFi network to enable the Flipper Zero to communicate over the network.

6. **Password (WiFi Network)**
   - Input the corresponding password for your WiFi network.

7. **Set File Type**
   - Enter your desired file extension. After saving, the app will rename your file, applying the new extension.

8. **Rename File**
   - Provide your desired file name. After saving, the app will rename your file with the new name.


## Saving Settings
After entering the desired configuration parameters, the app automatically saves these settings for use during the HTTP request process. You can update these settings at any time by navigating back to the **Settings** menu.

## Logging and Debugging
The Web Crawler app uses logging to help identify issues:

- **Info Logs**: Provide general information about the app's operations (e.g., UART initialization, sending settings).
- **Error Logs**: Indicate problems encountered during execution (e.g., failed to open settings file).

Connect your Flipper Zero to a computer and use a serial terminal to view these logs for detailed troubleshooting.

## Known Issues
1. **Screen Delay**: Occasionally, the Run screen may get stuck on "Receiving Data".
   - If it takes longer than 10 seconds, restart your Flipper Zero.

*Happy Crawling! 🕷️* 
