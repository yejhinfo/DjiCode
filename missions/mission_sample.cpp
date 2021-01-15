/*! @file mission_sample.cpp
 *  @version 3.3
 *  @date Jun 05 2017
 *
 *  @brief
 *  GPS Missions API usage in a Linux environment.
 *  Shows example usage of the Waypoint Missions and Hotpoint Missions through
 * the
 *  Mission Manager API.
 *
 *  @Copyright (c) 2017 DJI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "mission_sample.hpp"
#include "camera.hpp"


using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;
using namespace std;

int pic_num = 0;
int sum_piont = 0;

bool
setUpSubscription(DJI::OSDK::Vehicle* vehicle, int responseTimeout)
{
  // Telemetry: Verify the subscription
  ACK::ErrorCode subscribeStatus;

  subscribeStatus = vehicle->subscribe->verify(responseTimeout);
  if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(subscribeStatus, __func__);
    return false;
  }

  // Telemetry: Subscribe to flight status and mode at freq 10 Hz
  int       freq            = 10;
  TopicName topicList10Hz[] = { TOPIC_GPS_FUSED };
  int       numTopic        = sizeof(topicList10Hz) / sizeof(topicList10Hz[0]);
  bool      enableTimestamp = false;

  bool pkgStatus = vehicle->subscribe->initPackageFromTopicList(
    DEFAULT_PACKAGE_INDEX, numTopic, topicList10Hz, enableTimestamp, freq);
  if (!(pkgStatus))
  {
    return pkgStatus;
  }

  // Start listening to the telemetry data
  subscribeStatus =
    vehicle->subscribe->startPackage(DEFAULT_PACKAGE_INDEX, responseTimeout);
  if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(subscribeStatus, __func__);
    // Cleanup
    ACK::ErrorCode ack =
      vehicle->subscribe->removePackage(DEFAULT_PACKAGE_INDEX, responseTimeout);
    if (ACK::getError(ack))
    {
      std::cout << "Error unsubscribing; please restart the drone/FC to get "
        "back to a clean state.\n";
    }
    return false;
  }
  return true;
}

bool
teardownSubscription(DJI::OSDK::Vehicle* vehicle, const int pkgIndex,
                     int responseTimeout)
{
  ACK::ErrorCode ack =
    vehicle->subscribe->removePackage(pkgIndex, responseTimeout);
  if (ACK::getError(ack))
  {
    std::cout << "Error unsubscribing; please restart the drone/FC to get back "
      "to a clean state.\n";
    return false;
  }
  return true;
}


std::vector<DJI::OSDK::WayPointSettings>
inputOwnWaypoints(DJI::OSDK::Vehicle* vehicle, int numWaypoints)
{
	float waypointHeight;
	std::cout << "Set your waymission height: "; 
	std::cin >> waypointHeight;

	// create a vector to store waypoints
	std::vector<DJI::OSDK::WayPointSettings> wp_list;

	std::cout << "Now input your waypoints one by one.\n";
	for (int num = 1; num < numWaypoints; num++)
	{
		WayPointSettings wp;
		setWaypointDefaults(&wp);
		wp.index = num;
		std::cout << "Waypoint " << num << " : \n";
		std::cout << "wp's latitude: ";  std::cin >> wp.latitude;
		std::cout << "wp's longitude: "; std::cin >> wp.longitude;
		
		wp.altitude = waypointHeight;

		//wp.commandList[0] = show_speed;
		//wp.commandParameter[0] = vehicle;
		//wp.commandList[1] = show_gps;
		//wp.commandPatameter[1] = vehicle;
		
		//OpenProtocolCMD::CMDSet cmdset; 
		//wp.commandList[0] =	cmdset.mission.waypointGetVelocity;
		wp.commandList[0] = 0x17;
			
		wp_list.push_back(wp);
		printf("Waypoint created at (LLA): %f \t%f \t%f\n",
				wp.latitude, wp.longitude, wp.altitude);
	}

	return wp_list;
}
//拿速度
void
show_speed(DJI::OSDK::Vehicle* vehicle)
{
	Telemetry::Vector3f velocityData;
	velocityData = vehicle->broadcast->getVelocity();
	std::cout << "Velocity x: "   << velocityData.x 
			  << "\nVelocity y: " << velocityData.y
			  << "\nVelocity z: " << velocityData.z
			  << std::endl;
}

//拿gps
void 
show_gps(DJI::OSDK::Vehicle* vehicle)
{
	Telemetry::GlobalPosition data;
	data = vehicle->broadcast->getGlobalPosition();
	std::cout << "gps latitude: "    << data.latitude
			  << "\ngps longitude: " << data.longitude
			  << "\ngps altitude: "  << data.altitude
			  << std::endl;
}

bool
runWaypointMission(Vehicle* vehicle, int numWaypoints, int responseTimeout,struct post p[])
{
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    if (!setUpSubscription(vehicle, responseTimeout))
    {
      std::cout << "Failed to set up Subscription!" << std::endl;
      return false;
    }
    sleep(1);
  }

  // Waypoint Mission : Initialization
  WayPointInitSettings fdata;
  setWaypointInitDefaults(&fdata);

  fdata.indexNumber =
    numWaypoints + 1; // We add 1 to get the aircarft back to the start.

  float64_t increment = 0.000001;
  float32_t start_alt = 10;

  ACK::ErrorCode initAck = vehicle->missionManager->init(
    DJI_MISSION_TYPE::WAYPOINT, responseTimeout, &fdata);
  if (ACK::getError(initAck))
  {
    ACK::getErrorCodeMessage(initAck, __func__);
  }

  vehicle->missionManager->printInfo();
  std::cout << "Initializing Waypoint Mission..\n";

  // Waypoint Mission: Create Waypoints
  std::vector<WayPointSettings> generatedWaypts =
    //createWaypoints(vehicle, numWaypoints, increment, start_alt);
    mycreateWaypoints(vehicle, numWaypoints, increment, start_alt,p);
  std::cout << "Creating Waypoints..\n";

  // Waypoint Mission: Upload the waypoints
  uploadWaypoints(vehicle, generatedWaypts, responseTimeout);
  std::cout << "Uploading Waypoints..\n";

  // Waypoint Mission: Start
  ACK::ErrorCode startAck =
    vehicle->missionManager->wpMission->start(responseTimeout);
  if (ACK::getError(startAck))
  {
    ACK::getErrorCodeMessage(initAck, __func__);
  }
  else
  {
    std::cout << "Starting Waypoint Mission.\n";
  }
  sum_piont = numWaypoints*4;
  pic_num = 0;
  vehicle->missionManager->wpMission->setWaypointCallback(callback,NULL);
  // Cleanup before return. The mission isn't done yet, but it doesn't need any
  // more input from our side.
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    return teardownSubscription(vehicle, DEFAULT_PACKAGE_INDEX,
                                responseTimeout);
  }

  return true;
}

void
setWaypointDefaults(WayPointSettings* wp)
{
  wp->damping         = 0;
  wp->yaw             = 0;
  wp->gimbalPitch     = 0;
  wp->turnMode        = 0;
  wp->hasAction       = 0;
  wp->actionTimeLimit = 100;
  wp->actionNumber    = 0;
  wp->actionRepeat    = 0;
  for (int i = 0; i < 16; ++i)
  {
    wp->commandList[i]      = 0;
    wp->commandParameter[i] = 0;
  }
}

void
setWaypointInitDefaults(WayPointInitSettings* fdata)
{
  fdata->maxVelocity    = 10;
  fdata->idleVelocity   = 5;
  fdata->finishAction   = 0;
  fdata->executiveTimes = 1;
  fdata->yawMode        = 0;
  fdata->traceMode      = 0;
  fdata->RCLostAction   = 1;
  fdata->gimbalPitch    = 0;
  fdata->latitude       = 0;
  fdata->longitude      = 0;
  fdata->altitude       = 0;
}

std::vector<DJI::OSDK::WayPointSettings>
createWaypoints(DJI::OSDK::Vehicle* vehicle, int numWaypoints,
                float64_t distanceIncrement, float32_t start_alt)
{
  // Create Start Waypoint
  WayPointSettings start_wp;
  setWaypointDefaults(&start_wp);

  // Global position retrieved via subscription
  Telemetry::TypeMap<TOPIC_GPS_FUSED>::type subscribeGPosition;
  // Global position retrieved via broadcast
  Telemetry::GlobalPosition broadcastGPosition;

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    subscribeGPosition = vehicle->subscribe->getValue<TOPIC_GPS_FUSED>();
    start_wp.latitude  = subscribeGPosition.latitude;
    start_wp.longitude = subscribeGPosition.longitude;
    start_wp.altitude  = start_alt;
    printf("Waypoint created at (LLA): %f \t%f \t%f\n",
           subscribeGPosition.latitude, subscribeGPosition.longitude,
           start_alt);
  }
  else
  {
    broadcastGPosition = vehicle->broadcast->getGlobalPosition();
    start_wp.latitude  = broadcastGPosition.latitude;
    start_wp.longitude = broadcastGPosition.longitude;
    start_wp.altitude  = start_alt;
    printf("Waypoint created at (LLA): %f \t%f \t%f\n",
           broadcastGPosition.latitude, broadcastGPosition.longitude,
           start_alt);
  }

  std::vector<DJI::OSDK::WayPointSettings> wpVector =
    generateWaypointsPolygon(&start_wp, distanceIncrement, numWaypoints);
  return wpVector;
}


//修改这里的航线坐标就可以了
std::vector<DJI::OSDK::WayPointSettings>
generateWaypointsPolygon(WayPointSettings* start_data, float64_t increment,
                         int num_wp)
{

  // Let's create a vector to store our waypoints in.
  std::vector<DJI::OSDK::WayPointSettings> wp_list;

  // Some calculation for the polygon
  float64_t extAngle = 2 * M_PI / num_wp;

  // First waypoint
  start_data->index = 0;
  wp_list.push_back(*start_data);

  // Iterative algorithm
  for (int i = 1; i < num_wp; i++)
  {
    WayPointSettings  wp;
    WayPointSettings* prevWp = &wp_list[i - 1];
    setWaypointDefaults(&wp);
    wp.index     = i;
    //wp.latitude  = (prevWp->latitude + (increment * cos(i * extAngle)));
    //wp.longitude = (prevWp->longitude + (increment * sin(i * extAngle)));
    
    wp.latitude  = (prevWp->latitude +  (increment));
    wp.longitude = (prevWp->longitude + (increment));

    wp.altitude  = (prevWp->altitude + 1);
    wp_list.push_back(wp);

	// show wp's info
	std::cout << "WayPt info:\n"
			  << "index: " << wp.index
			  << "latitude: " << wp.latitude
			  << "longitude: " << wp.longitude
			  << "altitude: " << wp.altitude
			  << std::endl;
  }

  // Come back home
  start_data->index = num_wp;
  wp_list.push_back(*start_data);

  return wp_list;
}

void
uploadWaypoints(Vehicle*                                  vehicle,
                std::vector<DJI::OSDK::WayPointSettings>& wp_list,
                int                                       responseTimeout)
{
  for (std::vector<WayPointSettings>::iterator wp = wp_list.begin();
       wp != wp_list.end(); ++wp)
  {
    printf("Waypoint created at (LLA): %f \t%f \t%f\n ", wp->latitude,
           wp->longitude, wp->altitude);
    ACK::WayPointIndex wpDataACK =
      vehicle->missionManager->wpMission->uploadIndexData(&(*wp),
                                                          responseTimeout);

	//const char funcc;
	char funcc;
	std::cout << "Upload wps Error Status: " << std::endl;
    ACK::getErrorCodeMessage(wpDataACK.ack, __func__);
	//ACK::getErrorCodeMessage(wpDataACK.ack, funcc);
  }
}

bool
runHotpointMission(Vehicle* vehicle, int initialRadius, int responseTimeout)
{
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    if (!setUpSubscription(vehicle, responseTimeout))
    {
      std::cout << "Failed to set up Subscription!" << std::endl;
      return false;
    }
    sleep(1);
  }

  // Global position retrieved via subscription
  Telemetry::TypeMap<TOPIC_GPS_FUSED>::type subscribeGPosition;
  // Global position retrieved via broadcast
  Telemetry::GlobalPosition broadcastGPosition;

  // Hotpoint Mission Initialize
  vehicle->missionManager->init(DJI_MISSION_TYPE::HOTPOINT, responseTimeout,
                                NULL);
  vehicle->missionManager->printInfo();

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    subscribeGPosition = vehicle->subscribe->getValue<TOPIC_GPS_FUSED>();
    vehicle->missionManager->hpMission->setHotPoint(
      subscribeGPosition.longitude, subscribeGPosition.latitude, initialRadius);
  }
  else
  {
    broadcastGPosition = vehicle->broadcast->getGlobalPosition();
    vehicle->missionManager->hpMission->setHotPoint(
      broadcastGPosition.longitude, broadcastGPosition.latitude, initialRadius);
  }

  // Takeoff
  ACK::ErrorCode takeoffAck = vehicle->control->takeoff(responseTimeout);
  if (ACK::getError(takeoffAck))
  {
    ACK::getErrorCodeMessage(takeoffAck, __func__);

    if(takeoffAck.info.cmd_set == OpenProtocolCMD::CMDSet::control
       && takeoffAck.data == ErrorCode::CommonACK::START_MOTOR_FAIL_MOTOR_STARTED)
    {
      DSTATUS("Take off command sent failed. Please Land the drone and disarm the motors first.\n");
    }

    if (!vehicle->isM100() && !vehicle->isLegacyM600())
    {
      teardownSubscription(vehicle, DEFAULT_PACKAGE_INDEX, responseTimeout);
    }
    return false;
  }
  else
  {
    sleep(15);
  }

  // Start
  std::cout << "Start with default rotation rate: 15 deg/s" << std::endl;
  ACK::ErrorCode startAck =
    vehicle->missionManager->hpMission->start(responseTimeout);
  if (ACK::getError(startAck))
  {
    ACK::getErrorCodeMessage(startAck, __func__);
    if (!vehicle->isM100() && !vehicle->isLegacyM600())
    {
      teardownSubscription(vehicle, DEFAULT_PACKAGE_INDEX, responseTimeout);
    }
    return false;
  }
  sleep(20);

  // Pause
  std::cout << "Pause for 5s" << std::endl;
  ACK::ErrorCode pauseAck =
    vehicle->missionManager->hpMission->pause(responseTimeout);
  if (ACK::getError(pauseAck))
  {
    ACK::getErrorCodeMessage(pauseAck, __func__);
  }
  sleep(5);

  // Resume
  std::cout << "Resume" << std::endl;
  ACK::ErrorCode resumeAck =
    vehicle->missionManager->hpMission->resume(responseTimeout);
  if (ACK::getError(resumeAck))
  {
    ACK::getErrorCodeMessage(resumeAck, __func__);
  }
  sleep(10);

  // Update radius, no ACK
  std::cout << "Update radius to 1.5x: new radius = " << 1.5 * initialRadius
            << std::endl;
  vehicle->missionManager->hpMission->updateRadius(1.5 * initialRadius);
  sleep(10);

  // Update velocity (yawRate), no ACK
  std::cout << "Update hotpoint rotation rate: new rate = 5 deg/s" << std::endl;
  HotpointMission::YawRate yawRateStruct;
  yawRateStruct.clockwise = 1;
  yawRateStruct.yawRate   = 5;
  vehicle->missionManager->hpMission->updateYawRate(yawRateStruct);
  sleep(10);

  // Stop
  std::cout << "Stop" << std::endl;
  ACK::ErrorCode stopAck =
    vehicle->missionManager->hpMission->stop(responseTimeout);

  std::cout << "land" << std::endl;
  ACK::ErrorCode landAck = vehicle->control->land(responseTimeout);
  if (ACK::getError(landAck))
  {
    ACK::getErrorCodeMessage(landAck, __func__);
  }
  else
  {
    // No error. Wait for a few seconds to land
    sleep(10);
  }

  // Clean up
  ACK::getErrorCodeMessage(startAck, __func__);
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    teardownSubscription(vehicle, DEFAULT_PACKAGE_INDEX, responseTimeout);
  }

  return true;
}

/*********************************************************/
//add

/*! Monitored Takeoff (Blocking API call). Return status as well as ack.
    This version of takeoff makes sure your aircraft actually took off
    and only returns when takeoff is complete.
    Use unless you want to do other stuff during takeoff - this will block
    the main thread.
!*/
bool
monitoredLanding(Vehicle* vehicle, int timeout)
{
  //@todo: remove this once the getErrorCode function signature changes
  char func[50];
  int  pkgIndex;

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    // Telemetry: Verify the subscription
    ACK::ErrorCode subscribeStatus;
    subscribeStatus = vehicle->subscribe->verify(timeout);
    if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
    {
      ACK::getErrorCodeMessage(subscribeStatus, func);
      return false;
    }

    // Telemetry: Subscribe to flight status and mode at freq 10 Hz
    pkgIndex                  = 0;
    int       freq            = 10;
    TopicName topicList10Hz[] = { TOPIC_STATUS_FLIGHT,
                                  TOPIC_STATUS_DISPLAYMODE };
    int  numTopic        = sizeof(topicList10Hz) / sizeof(topicList10Hz[0]);
    bool enableTimestamp = false;

    bool pkgStatus = vehicle->subscribe->initPackageFromTopicList(
      pkgIndex, numTopic, topicList10Hz, enableTimestamp, freq);
    if (!(pkgStatus))
    {
      return pkgStatus;
    }
    subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, timeout);
    if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
    {
      ACK::getErrorCodeMessage(subscribeStatus, func);
      // Cleanup before return
      vehicle->subscribe->removePackage(pkgIndex, timeout);
      return false;
    }
  }

  // Start landing
  ACK::ErrorCode landingStatus = vehicle->control->land(timeout);
  if (ACK::getError(landingStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(landingStatus, func);
    return false;
  }

  // First check: Landing started
  int landingNotStarted = 0;
  int timeoutCycles     = 20;

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    while (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
             VehicleStatus::DisplayMode::MODE_AUTO_LANDING &&
           landingNotStarted < timeoutCycles)
    {
      landingNotStarted++;
      usleep(100000);
    }
  }
  else if (vehicle->isM100())
  {
    while (vehicle->broadcast->getStatus().flight !=
             DJI::OSDK::VehicleStatus::M100FlightStatus::LANDING &&
           landingNotStarted < timeoutCycles)
    {
      landingNotStarted++;
      usleep(100000);
    }
  }

  if (landingNotStarted == timeoutCycles)
  {
    std::cout << "Landing failed. Aircraft is still in the air." << std::endl;
    if (!vehicle->isM100() && !vehicle->isLegacyM600())
    {
      // Cleanup before return
      ACK::ErrorCode ack = vehicle->subscribe->removePackage(pkgIndex, timeout);
      if (ACK::getError(ack)) {
        std::cout << "Error unsubscribing; please restart the drone/FC to get "
                     "back to a clean state.\n";
      }
    }
    return false;
  }
  else
  {
    std::cout << "Landing...\n";
  }

  // Second check: Finished landing
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    while (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() ==
             VehicleStatus::DisplayMode::MODE_AUTO_LANDING &&
           vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>() ==
             VehicleStatus::FlightStatus::IN_AIR)
    {
      sleep(1);
    }

    if (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
          VehicleStatus::DisplayMode::MODE_P_GPS ||
        vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
          VehicleStatus::DisplayMode::MODE_ATTITUDE)
    {
      std::cout << "Successful landing!\n";
    }
    else
    {
      std::cout
        << "Landing finished, but the aircraft is in an unexpected mode. "
           "Please connect DJI GO.\n";
      ACK::ErrorCode ack = vehicle->subscribe->removePackage(pkgIndex, timeout);
      if (ACK::getError(ack))
      {
        std::cout << "Error unsubscribing; please restart the drone/FC to get "
                     "back to a clean state.\n";
      }
      return false;
    }
  }
  else if (vehicle->isLegacyM600())
  {
    while (vehicle->broadcast->getStatus().flight >
           DJI::OSDK::VehicleStatus::FlightStatus::STOPED)
    {
      sleep(1);
    }

    Telemetry::GlobalPosition gp;
    do
    {
      sleep(2);
      gp = vehicle->broadcast->getGlobalPosition();
    std::cout << "landing info:\n"
          << "altitude: " << gp.altitude
        << std::endl;
    } while (gp.altitude != 0);

    if (gp.altitude != 0) // thid logic has some problems
    {
      std::cout
        << "Landing finished, but the aircraft is in an unexpected mode. "
           "Please connect DJI GO.\n";
      return false;
    }
    else
    {
      std::cout << "Successful landing!\n";
    }
  }
  else // M100
  {
    while (vehicle->broadcast->getStatus().flight ==
           DJI::OSDK::VehicleStatus::M100FlightStatus::FINISHING_LANDING)
    {
      sleep(1);
    }

    Telemetry::GlobalPosition gp;
    do
    {
      sleep(2);
      gp = vehicle->broadcast->getGlobalPosition();
    } while (gp.altitude != 0);

    if (gp.altitude != 0)
    {
      std::cout
        << "Landing finished, but the aircraft is in an unexpected mode. "
           "Please connect DJI GO.\n";
      return false;
    }
    else
    {
      std::cout << "Successful landing!\n";
    }
  }

  // Cleanup
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    ACK::ErrorCode ack = vehicle->subscribe->removePackage(pkgIndex, timeout);
    if (ACK::getError(ack))
    {
      std::cout
        << "Error unsubscribing; please restart the drone/FC to get back "
           "to a clean state.\n";
    }
  }

  return true;
}


/*! Monitored Takeoff (Blocking API call). Return status as well as ack.
    This version of takeoff makes sure your aircraft actually took off
    and only returns when takeoff is complete.
    Use unless you want to do other stuff during takeoff - this will block
    the main thread.
!*/
bool
monitoredTakeoff(Vehicle* vehicle, int timeout)
{
  //@todo: remove this once the getErrorCode function signature changes
  char func[50];
  int  pkgIndex;

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    // Telemetry: Verify the subscription
    ACK::ErrorCode subscribeStatus;
    subscribeStatus = vehicle->subscribe->verify(timeout);
    if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
    {
      ACK::getErrorCodeMessage(subscribeStatus, func);
      return false;
    }

    // Telemetry: Subscribe to flight status and mode at freq 10 Hz
    pkgIndex                  = 0;
    int       freq            = 10;
    TopicName topicList10Hz[] = { TOPIC_STATUS_FLIGHT,
                                  TOPIC_STATUS_DISPLAYMODE };
    int  numTopic        = sizeof(topicList10Hz) / sizeof(topicList10Hz[0]);
    bool enableTimestamp = false;

    bool pkgStatus = vehicle->subscribe->initPackageFromTopicList(
      pkgIndex, numTopic, topicList10Hz, enableTimestamp, freq);
    if (!(pkgStatus))
    {
      return pkgStatus;
    }
    subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, timeout);
    if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
    {
      ACK::getErrorCodeMessage(subscribeStatus, func);
      // Cleanup before return
      vehicle->subscribe->removePackage(pkgIndex, timeout);
      return false;
    }
  }

  // Start takeoff
  ACK::ErrorCode takeoffStatus = vehicle->control->takeoff(timeout);
  if (ACK::getError(takeoffStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(takeoffStatus, func);
    return false;
  }

  // First check: Motors started
  int motorsNotStarted = 0;
  int timeoutCycles    = 20;

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    while (vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>() !=
             VehicleStatus::FlightStatus::ON_GROUND &&
           vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
             VehicleStatus::DisplayMode::MODE_ENGINE_START &&
           motorsNotStarted < timeoutCycles)
    {
      motorsNotStarted++;
      usleep(100000);
    }

    if (motorsNotStarted == timeoutCycles)
    {
      std::cout << "Takeoff failed. Motors are not spinning." << std::endl;
      // Cleanup
      if (!vehicle->isM100() && !vehicle->isLegacyM600())
      {
        vehicle->subscribe->removePackage(0, timeout);
      }
      return false;
    }
    else
    {
      std::cout << "Motors spinning...\n";
    }
  }
  else if (vehicle->isLegacyM600())
  {
    while ((vehicle->broadcast->getStatus().flight <
            DJI::OSDK::VehicleStatus::FlightStatus::ON_GROUND) &&
           motorsNotStarted < timeoutCycles)
    {
      motorsNotStarted++;
      usleep(100000);
    }

    if (motorsNotStarted < timeoutCycles)
    {
      std::cout << "Successful TakeOff!" << std::endl;
    }
  }
  else // M100
  {
    while ((vehicle->broadcast->getStatus().flight <
            DJI::OSDK::VehicleStatus::M100FlightStatus::TAKEOFF) &&
           motorsNotStarted < timeoutCycles)
    {
      motorsNotStarted++;
      usleep(100000);
    }

    if (motorsNotStarted < timeoutCycles)
    {
      std::cout << "Successful TakeOff!" << std::endl;
    }
  }

  // Second check: In air
  int stillOnGround = 0;
  timeoutCycles     = 110;

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    while (vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>() !=
             VehicleStatus::FlightStatus::IN_AIR &&
           (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
              VehicleStatus::DisplayMode::MODE_ASSISTED_TAKEOFF ||
            vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
              VehicleStatus::DisplayMode::MODE_AUTO_TAKEOFF) &&
           stillOnGround < timeoutCycles)
    {
      stillOnGround++;
      usleep(100000);
    }

    if (stillOnGround == timeoutCycles)
    {
      std::cout << "Takeoff failed. Aircraft is still on the ground, but the "
                   "motors are spinning."
                << std::endl;
      // Cleanup
      if (!vehicle->isM100() && !vehicle->isLegacyM600())
      {
        vehicle->subscribe->removePackage(0, timeout);
      }
      return false;
    }
    else
    {
      std::cout << "Ascending...\n";
    }
  }
  else if (vehicle->isLegacyM600())
  {
    while ((vehicle->broadcast->getStatus().flight <
            DJI::OSDK::VehicleStatus::FlightStatus::IN_AIR) &&
           stillOnGround < timeoutCycles)
    {
      stillOnGround++;
      usleep(100000);
    }

    if (stillOnGround < timeoutCycles)
    {
      std::cout << "Aircraft in air!" << std::endl;
    }
  }
  else // M100
  {
    while ((vehicle->broadcast->getStatus().flight !=
            DJI::OSDK::VehicleStatus::M100FlightStatus::IN_AIR_STANDBY) &&
           stillOnGround < timeoutCycles)
    {
      stillOnGround++;
      usleep(100000);
    }

    if (stillOnGround < timeoutCycles)
    {
      std::cout << "Aircraft in air!" << std::endl;
    }
  }

  // Final check: Finished takeoff
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    while (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() ==
             VehicleStatus::DisplayMode::MODE_ASSISTED_TAKEOFF ||
           vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() ==
             VehicleStatus::DisplayMode::MODE_AUTO_TAKEOFF)
    {
      sleep(1);
    }

    if (!vehicle->isM100() && !vehicle->isLegacyM600())
    {
      if (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
            VehicleStatus::DisplayMode::MODE_P_GPS ||
          vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
            VehicleStatus::DisplayMode::MODE_ATTITUDE)
      {
        std::cout << "Successful takeoff!\n";
      }
      else
      {
        std::cout
          << "Takeoff finished, but the aircraft is in an unexpected mode. "
             "Please connect DJI GO.\n";
        vehicle->subscribe->removePackage(0, timeout);
        return false;
      }
    }
  }
  else
  {
    float32_t                 delta;
    Telemetry::GlobalPosition currentHeight;
    Telemetry::GlobalPosition deltaHeight =
      vehicle->broadcast->getGlobalPosition();

    do
    {
      sleep(4);
      currentHeight = vehicle->broadcast->getGlobalPosition();
      delta         = fabs(currentHeight.altitude - deltaHeight.altitude);
      deltaHeight.altitude = currentHeight.altitude;

    // show it's current point info
    std::cout << "Current latitude: "    << currentHeight.latitude
          << "\nCurrent longitude: " << currentHeight.longitude
          << "\nCurrent altitude: "  << currentHeight.altitude
          << "\nCurrent height: "    << currentHeight.height
          << "\nCurrent health: "    << currentHeight.health
              << std::endl;

    } while (delta >= 0.009);

    std::cout << "Aircraft hovering at " << currentHeight.altitude << "m!\n";
  }

  // Cleanup
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    ACK::ErrorCode ack = vehicle->subscribe->removePackage(pkgIndex, timeout);
    if (ACK::getError(ack))
    {
      std::cout
        << "Error unsubscribing; please restart the drone/FC to get back "
           "to a clean state.\n";
    }
  }

  return true;
}

void callback(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
  int reached = 6;
 // if(recvFrame.recvData.wayPointStatusPushData.current_status==reached && sum_piont!=pic_num){
 //   show_gps(vehicle);
 // }
  /*! 0: pre-mission, 1: in-action, 5: first waypt , 6: reached */
 if(recvFrame.recvData.wayPointStatusPushData.current_status==reached && sum_piont!=pic_num){
 	std::cout<<recvFrame.recvData.wayPointStatusPushData.current_status<<endl;
 	for(int i=1;i<=4;i++){
 		camera_pic(pic_num);
 		pic_num++;
 	}
 }
  //std::cout<<pic_num<<endl;
}

float64_t charToFloat(char hex_buf[],int len){
	int j = 0;
	int k = 0;
	char t[21];
	float64_t res = 0;
	for(int i=0;i<len;i++){
		if(hex_buf[i]==':')break;
		k++;
	}
	for(int i=k+1;i<len;i++){
		if(hex_buf[i]=='!')break;
		t[j] = hex_buf[i];
		j++;
	}

	res = atof(t);
	printf("%lf\n", res);
	return res;
}

int addPostLatitude(char hex_buf[],int len,struct post p[]){
	int k = (hex_buf[2]-'0')*10+hex_buf[3]-'0';
	p[k].latitude = charToFloat(hex_buf,len);
	cout<<"k: "<<k<<" "<<"p[k].latitude: "<<p[k].latitude<<endl;
	return k;
}

int addPostLongitude(char hex_buf[],int len,struct post p[]){
	int k = (hex_buf[2]-'0')*10+hex_buf[3]-'0';
	p[k].longitude = charToFloat(hex_buf,len);
	cout<<"k: "<<k<<" "<<"p[k].longitude: "<<p[k].longitude<<endl;
	return k;
}

std::vector<DJI::OSDK::WayPointSettings>
mycreateWaypoints(DJI::OSDK::Vehicle* vehicle, int numWaypoints,
                float64_t distanceIncrement, float32_t start_alt,struct post p[])
{
  // Create Start Waypoint
  WayPointSettings start_wp;
  setWaypointDefaults(&start_wp);
  Telemetry::TypeMap<TOPIC_GPS_FUSED>::type subscribeGPosition;


  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
  // subscribeGPosition = vehicle->subscribe->getValue<TOPIC_GPS_FUSED>();
  // start_wp.latitude  = subscribeGPosition.latitude;
  // start_wp.longitude = subscribeGPosition.longitude;
    start_wp.latitude  = p[0].latitude;
    start_wp.longitude = p[0].longitude;
    start_wp.altitude  = start_alt;
    printf("Waypoint created at (LLA) subscribeGPosition: %f \t%f \t%f\n",
           subscribeGPosition.latitude, subscribeGPosition.longitude,
           start_alt);
  }
  
  std::vector<DJI::OSDK::WayPointSettings> wpVector =
    mygenerateWaypointsPolygon(&start_wp, distanceIncrement, numWaypoints,p);
  return wpVector;
}

std::vector<DJI::OSDK::WayPointSettings>
mygenerateWaypointsPolygon(WayPointSettings* start_data, float64_t increment,
                         int num_wp,struct post p[])
{

  // Let's create a vector to store our waypoints in.
  std::vector<DJI::OSDK::WayPointSettings> wp_list;

  // First waypoint
  start_data->index = 0;
  wp_list.push_back(*start_data);

  printf("---num_wp--- : %d \n", num_wp);
  // Iterative algorithm
  for (int i = 1; i < num_wp; i++)
  {
    WayPointSettings  wp;
    setWaypointDefaults(&wp);
    wp.index     = i;
    wp.latitude  = p[i].latitude;
    wp.longitude = p[i].longitude;
    wp.altitude  = 10;
    wp_list.push_back(wp);

	// show wp's info
	std::cout << "WayPt info:\n"
			  << "index: " << wp.index
			  << "latitude: " << wp.latitude
			  << "longitude: " << wp.longitude
			  << "altitude: " << wp.altitude
			  << std::endl;
  }

  // Come back home
  start_data->index = num_wp;
  wp_list.push_back(*start_data);

  return wp_list;
}










