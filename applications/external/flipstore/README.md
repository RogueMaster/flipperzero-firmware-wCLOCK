# FlipStore
Download Flipper Zero apps directly to your Flipper Zero using WiFi. You no longer need another device to install apps. FlipStore uses the FlipperHTTP flash for the WiFi Devboard, first introduced in the WebCrawler app: https://github.com/jblanked/WebCrawler-FlipperZero/tree/main/assets/FlipperHTTP

## Features
- App Catalog
- Install Apps
- Delete Apps (coming soon)
- Install Custom Apps (coming soon)
- Install Devboard Flashes (coming soon)
- Install Official Firmware (coming soon)

## Installation
1. Flash your WiFi Devboard: https://github.com/jblanked/FlipperHTTP
2. Install the app.
3. Enjoy :D

## Roadmap
**v0.2**
- Stability Patch
- App Categories

**v0.3**
- Improved memory allocation
- Stability Patch 2
- App Catalog Patch (add in required functionalility)

**v0.4**
- Delete Apps

**v0.5**
- App short description
- App version

**v0.6**
- Download flash firmware (Marauder, Black Magic, FlipperHTTP)

**v0.7**
- Download custom apps from a GitHub URL

**v0.8**
- App Icons

**1.0**
- Download Official Firmware/Firmware Updates

## Contribution
This is a big task, and I welcome all contributors, especially developers interested in animations and graphics. Fork the repository, create a pull request, and I will review your edits.

## Known Bugs
1. Clicking the catalog results in an "Out of Memory" error.
   - This issue has been addressed, but it may still occur. If it does, restart the app.
2. The app file is corrupted.
   - This is likely due to an error parsing the data. Restart the app and wait until the green LED light turns off after downloading the app before exiting the view. If this happens more than three times, the current version of FlipStore may not be able to download that app successfully.
3. The app is frozen on the "Installing" or "Receiving data" screen. 
   - If it there LED is not on and it's been more than 5 seconds, restart your Flipper Zero with the devboard plugged in.
