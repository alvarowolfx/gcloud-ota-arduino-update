# Serverless Continuous Integration and OTA update flow using github actions and platformio cli end google cloud storage.

This repo offers two different ways to compile and upload the compiled binary to the google cloud bucket:
- online compilation with github actions
- offline compilation and upload to cloud via script

### Setup online compilation mode:
- clone this repo
- Setup a google cloud account
- Create a google cloud storage
- get the authentication key in a json
- get the id of the bucket
- set the permission of the bucket as viewable by all
- Create two secrets in the github repo called `APPLICATION_CREDENTIALS` and `PROJECT_ID`
- set the `PROJECT_ID` key with the project id from google cloud bucket
- set the `APPLICATION_CREDENTIALS` key with the base 64 encode of the key json by running this command in the terminal `base64 ~/<account_id>.json` as described [here](https://github.com/actions-hub/gcloud)

### Setup offline compilation mode + remote deployment:
 - clone the repo
 - cd to inside the repo folder
 - install virtualenvironment `./scripts/setupVenv.sh`
 - setup the venv by running `./scripts/setupVenv.sh` this will create a python venv in the root of the project and install the needed dependencies
 - setup google cloud bucket and configure authentication as described [here](https://pypi.org/project/google-cloud-storage/)
 - download the authentication key in json format, rename it as auth.json and move it to the scripts folder.

### Compile using the cloud compilation mode
- just commit and push your code to the repo, this will trigger the all workflow

### Compile using the offline mode
- from the project root folder run the compile and upload script `./scripts/compileAndUpload.sh`

### Check that the binary was uploaded correctly
- Visit the bucket url https://storage.googleapis.com/remote-esp32-upload-firmwares/

### Setup PlatformIO
- after installing the platform io ide plugin for vscode or atom install the platformio [shell commands](https://docs.platformio.org/en/latest/core/installation.html#install-shell-commands)
- open the terminal and add this line export `PATH=$PATH:~/.platformio/penv/bin` to your `~/.bash_profile`
```sudo nano ~/.bash_profile```
- after restarting the editor you should be able to use platformio from command line.
- manually install the this two libraries from the command line (needed because of [this](https://docs.platformio.org/en/latest/integration/ide/vscode.html#platformio-ide-usedevelopmentpiocore) known bug ):
```platformio lib install NTPClient@3.1.0```
```platformio lib install ArduinoJson@6.15.1```

### editing the Arduino code
- the main arduino code is in the folder ESP32_OTA_test
- make sure you set the correct wifi credentials otherwise the board won't be able to connect to the server and check for updates.
- if you change the folder and arduino name, make sure you make chages to the platformio.ini file

### WARNING
- the bucket I configured automatically deletes uploaded binary files after 2 days
- to limit the amount of result in the query when looking for new files I'm filtering the binary based on the hour of creation; It might happen that if you run the deployment script at 12.59, the arduinos won't see the new file it and you have to run this again.

### References
* highly inspired by https://medium.com/google-cloud/serverless-continuous-integration-and-ota-update-flow-using-google-cloud-build-and-arduino-d5e1cda504bf
