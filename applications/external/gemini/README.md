# Gemini-Flipper

Currently in development, anyone who wants to join and help improve it can make their PR and also for those C developers who want to help create the native Flipper app and make it accessible to everyone from https://lab.flipper.net/apps is welcome 

You can find the first version in the release section.

Special thanks to Derek Jamison who created a large part and I would dare say the entire application in C thanks to his work and effort (I only made some small changes that I would never have achieved without his help and tutorials) he deserves all the recognition for this 👏👏 https://github.com/jamisonderek

Important: Due to changes in the Flipper Zero firmware regarding the implementation of mJs and the changes it had, the .js application no longer works with the new firmware versions, but do not worry the native C application is now ready for everyone to enjoy

## How it works

Currently the code takes the api key in plain text stored in:

"/ext/apps_data/gemini_ia/key.txt" <-- You need to create it manually

You can get the Gemini Api Key using this link: https://ai.google.dev/gemini-api/docs/api-key You only need a Gmail account

it reads the .txt file and sends it via serial command to the esp32 as soon as the app is initialized (it would be nice to implement some kind of encryption to the api key but it is important that it is sent in plain text and unencrypted to the esp32 because that is how the firmware can read and implement it)

If you previously connected to a Wi-Fi network it will automatically reconnect and you can continue chatting without having to reconnect to the Wi-Fi because it stores known networks and connects if you are near one

I added the source code for the new firmware for the esp32 and also the precompiled binary for the ESP-Flasher app (you can find the binary in the releases section)

## How to flash the firmware using ESP-Flasher

Check the folder "Gemini IA Precompiled binaries" there is a tutorial on how to do it there

## TO DO

Remove unnecessary files <-- Done

Create a Dev branch to avoid conflicts with the Main branch <-- Done

Rewrite javascript app to work with new firmware <-- Done

Javascript: Change menu to leave only the "connect to a new AP" option the "saved APs" option is deprecated, now the list of saved APs is sent in the background to the ESP32 after setting the name so it will detect if any of the networks are available and automatically connect to it, otherwise it will prompt the user to enter the username and password of the AP they want to connect to and save the data for this new AP for the future <-- Done

Rewrite ESP32 firmware should now work with all ESP32 models using ESP-TOOLS <-- Done

Improve handling of how APs are saved, currently it saves them but rewrites the old one <-- Done

Improve handling of how saved APs are sent to the esp32 to avoid re-entering the SSID and password (Improving the previous point this should fix itself) <-- Done

upload firmware binary for esp32 (nothing to do here just upload binary) <-- Done

## Native App

The native app is under development. Currently it just shows the main menu and then every option shows "Under Construction".

~~Changes: Now you can not only set your name and chat, but you can also change the name if you want. We still need to implement the feature to connect to a new AP (currently it only connects to those already saved in "/ext/apps_data/gemini_ia/SavedAPs.txt"). Side note: I'll make a little tutorial on how to use the app once everything is stable and ready for everyone to use.~~

Changes: The native app is now 100% working, the only problem is that I still can't get the board to reboot when closing the app (so you don't have any issues when reopening the app) <-- every time you want to enter the app you'll have to press the reset button on your esp32 board first (a tutorial coming soon)

## Contributors

https://github.com/jamisonderek
