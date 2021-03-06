/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <ctime>
 #include <stdio.h>
 #include <curl/curl.h>
#include "Leap.h"

using namespace Leap;

class SampleListener : public Listener {
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);

  private:
};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void SampleListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}


int timestamp = 0;
int last_type = -1;

void SampleListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
   const Frame frame = controller.frame();
  const GestureList gestures = frame.gestures();
CURL *curl;
   CURLcode res;
  for (int g = 0; g < gestures.count(); ++g) {
    Gesture gesture = gestures[g];
    switch (gesture.type()) {
     
      case Gesture::TYPE_SWIPE:
      {
        SwipeGesture swipe = gesture;
        if (swipe.direction()[0] < 0.4 && swipe.direction()[0] > -0.4) continue;
	
        if(swipe.direction()[0]<0) {
            if(last_type == 0 && time(NULL)-timestamp < 2) continue;
            std::cout << "SWIPED LEFT";
            last_type = 0;
		if(curl) {
			curl = curl_easy_init();
  	  		 curl_easy_setopt(curl, CURLOPT_URL, "http://52.178.154.133:8080/actions/swipe_left");
   			  res = curl_easy_perform(curl);
   	 		  /* always cleanup */
   			 curl_easy_cleanup(curl);
  	 }
        }
        else {
            if(last_type == 1 && time(NULL)-timestamp < 2) continue;
            std::cout << "SWIPED RIGHT";
            last_type = 1;
		if(curl) {
			curl = curl_easy_init();
  	   		curl_easy_setopt(curl, CURLOPT_URL, "http://52.178.154.133:8080/actions/swipe_right");
   	 		 res = curl_easy_perform(curl);
   			   /* always cleanup */
   			 curl_easy_cleanup(curl);
  	 }
        }
        timestamp = time(NULL);

        std::cout << std::endl;
        break;
      }
      default:
      //  std::cout << std::string(2, ' ')  << "Unknown gesture type." << std::endl;
        break;
    }
  }

  if (!frame.hands().isEmpty() || !gestures.isEmpty()) {
    // std::cout << std::endl;
  }

}

void SampleListener::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
  std::cout << "Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    // std::cout << "id: " << devices[i].toString() << std::endl;
    // std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
}

void SampleListener::onServiceConnect(const Controller& controller) {
  std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
  std::cout << "Service Disconnected" << std::endl;
}

int main(int argc, char** argv) {
  // Create a sample listener and controller
  SampleListener listener;
  Controller controller;

  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  if (argc > 1 && strcmp(argv[1], "--bg") == 0)
    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

  // Keep this process running until Enter is pressed
  std::cout << "Press Enter to quit..." << std::endl;
  std::cin.get();

  // Remove the sample listener when done
  controller.removeListener(listener);

  return 0;
}
