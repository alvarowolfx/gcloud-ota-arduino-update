# Serverless Continuous Integration and OTA update flow using Google Cloud Build and Arduino

How to build your firmware continuously in the cloud and sent to your devices automatically.

Blog post with details: https://medium.com/google-cloud/serverless-continuous-integration-and-ota-update-flow-using-google-cloud-build-and-arduino-d5e1cda504bf

### Setup Google cloud tools and project

* Install beta components:
    * `gcloud components install beta`
* Authenticate with Google Cloud:
    * `gcloud auth login`
* Create cloud project — choose your unique project name:
    * `gcloud projects create YOUR_PROJECT_NAME`
* Set current project
    * `gcloud config set project YOUR_PROJECT_NAME`

* Cloud Build Setup :
  * Open the Cloud Build page.
  * Enable Cloud Build API if not enabled yet.
  * Select the Triggers tab.
  * Click on Create Trigger.
  * Now you can select your Git repository and authenticate with your provider. I used Github.
  * On the last step, Trigger Settings, let’s create a trigger by git tags.
  * Name it — “Trigger by Build Tag”.
  * Select Tag as the Trigger type.
  * Select cloudbuild.yaml as the Build configuration type.
  * Click on Create Trigger.

* Cloud Storage Setup :
  * Now Open the Cloud Storage page
  * Select the Browser Tab.
  * Click on Create bucket.
  * Create a bucket with the name YOUR_PROJECT_NAME-firmwaresand select the let the other values as the defaults. In my case, the name of the bucket was gcloud-ota-update-firmwares.
  * Click on Create.

### Deploy Cloud Functions

* Run on the command line:
```
./deploy-prod.sh
```

### Upload firmware with PlatfomIO

I recommend installing the Visual Studio Code (VSCode) IDE and the PlatformIO plugin to get started using it. Just follow the step on the link below: 

https://platformio.org/platformio-ide

To deploy to the board you can use the “Build” and “Upload” buttons on PlatformIO Toolbar.


### References

*  https://dzone.com/articles/how-to-approach-ota-updates-for-iot
* https://cloud.google.com/functions/docs/writing
* https://cloud.google.com/functions/docs/tutorials/storage
* http://docs.platformio.org/en/latest